/*
 * Copyright (c) 2021, Hunter Salyer <thefalsehonesty@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DebuggerGlobalJSObject.h"
#include "Debugger.h"
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/ProxyObject.h>

namespace HackStudio {

DebuggerGlobalJSObject::DebuggerGlobalJSObject()
{
    auto regs = Debugger::the().session()->get_registers();
    auto lib = Debugger::the().session()->library_at(regs.eip);
    if (!lib)
        return;
    m_variables = lib->debug_info->get_variables_in_current_scope(regs);
}

JS::Value DebuggerGlobalJSObject::get(const JS::PropertyName& name, JS::Value receiver, bool without_side_effects) const
{
    if (m_variables.is_empty() || !name.is_string())
        return JS::Object::get(name, receiver, without_side_effects);

    auto it = m_variables.find_if([&](auto& variable) {
        return variable->name == name.as_string();
    });
    if (it.is_end())
        return JS::Object::get(name, receiver, without_side_effects);
    auto& target_variable = **it;
    auto js_value = debugger_to_js(target_variable);
    if (js_value.has_value())
        return js_value.value();
    auto error_string = String::formatted("Variable {} of type {} is not convertible to a JS Value", name.as_string(), target_variable.type_name);
    vm().throw_exception<JS::TypeError>(const_cast<DebuggerGlobalJSObject&>(*this), error_string);
    return {};
}

bool DebuggerGlobalJSObject::put(const JS::PropertyName& name, JS::Value value, JS::Value receiver)
{
    if (m_variables.is_empty() || !name.is_string())
        return JS::Object::put(name, value, receiver);

    auto it = m_variables.find_if([&](auto& variable) {
        return variable->name == name.as_string();
    });
    if (it.is_end())
        return JS::Object::put(name, value, receiver);
    auto& target_variable = **it;
    auto debugger_value = js_to_debugger(value, target_variable);
    if (debugger_value.has_value())
        return Debugger::the().session()->poke((u32*)target_variable.location_data.address, debugger_value.value());
    auto error_string = String::formatted("Cannot convert JS value {} to variable {} of type {}", value.to_string_without_side_effects(), name.as_string(), target_variable.type_name);
    vm().throw_exception<JS::TypeError>(const_cast<DebuggerGlobalJSObject&>(*this), error_string);
    return {};
}

Optional<JS::Value> DebuggerGlobalJSObject::debugger_to_js(const Debug::DebugInfo::VariableInfo& variable) const
{
    if (variable.location_type != Debug::DebugInfo::VariableInfo::LocationType::Address)
        return {};

    auto variable_address = variable.location_data.address;

    if (variable.is_enum_type() || variable.type_name == "int") {
        auto value = Debugger::the().session()->peek((u32*)variable_address);
        VERIFY(value.has_value());
        return JS::Value((i32)value.value());
    }

    if (variable.type_name == "char") {
        auto value = Debugger::the().session()->peek((u32*)variable_address);
        VERIFY(value.has_value());
        return JS::Value((char)value.value());
    }

    if (variable.type_name == "bool") {
        auto value = Debugger::the().session()->peek((u32*)variable_address);
        VERIFY(value.has_value());
        return JS::Value(value.value() != 0);
    }

    auto& global = const_cast<DebuggerGlobalJSObject&>(*this);

    auto* object = JS::Object::create_empty(global);
    auto* handler = JS::Object::create_empty(global);
    auto proxy = JS::ProxyObject::create(global, *object, *handler);

    auto set = [&](JS::VM& vm, JS::GlobalObject&) {
        auto property = vm.argument(1).value_or(JS::js_undefined());
        if (!property.is_string())
            return JS::Value(false);
        auto property_name = property.as_string().string();

        auto value = vm.argument(2).value_or(JS::js_undefined());
        dbgln("prop name {}", property_name);

        auto it = variable.members.find_if([&](auto& variable) {
            dbgln("candidate debugger var name: {}", variable->name);
            return variable->name == property_name;
        });
        if (it.is_end())
            return JS::Value(false);
        auto& member = **it;
        dbgln("Found var {}", member.name);

        auto new_value = js_to_debugger(value, member);
        Debugger::the().session()->poke((u32*)member.location_data.address, new_value.value());

        return JS::Value(true);
    };

    handler->define_native_function("set", move(set), 4);

    for (auto& member : variable.members) {
        auto member_value = debugger_to_js(member);
        if (!member_value.has_value())
            continue;
        object->put(member.name, member_value.value());
    }
    return proxy;
}

Optional<u32> DebuggerGlobalJSObject::js_to_debugger(JS::Value value, const Debug::DebugInfo::VariableInfo& variable) const
{
    if (value.is_string() && variable.type_name == "char") {
        auto string = value.as_string().string();
        if (string.length() != 1)
            return {};
        return string[0];
    }

    if (value.is_number() && (variable.is_enum_type() || variable.type_name == "int"))
        return value.as_u32();

    if (value.is_boolean() && variable.type_name == "bool")
        return value.as_bool();

    return {};
}

}
