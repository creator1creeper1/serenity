/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/FlyString.h>
#include <AK/Function.h>
#include <LibJS/Interpreter.h>
#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/MathObject.h>

namespace JS {

MathObject::MathObject()
{
    put_native_function("abs", abs);
    put_native_function("random", random);
}

MathObject::~MathObject()
{
}

Value MathObject::abs(Interpreter& interpreter)
{
    if (interpreter.call_frame().arguments.is_empty())
        return js_nan();

    auto argument = interpreter.call_frame().arguments[0];

    if (argument.is_array()) {
        auto& array = *static_cast<const Array*>(argument.as_object());
        if (array.length() == 0)
            return Value(0);
        if (array.length() > 1)
            return js_nan();
        argument = array.elements()[0];
    }

    auto number = argument.to_number();
    if (number.is_nan())
        return js_nan();
    return Value(number.as_double() >= 0 ? number.as_double() : -number.as_double());
}

Value MathObject::random(Interpreter&)
{
#ifdef __serenity__
    double r = (double)arc4random() / (double)UINT32_MAX;
#else
    double r = (double)rand() / (double)RAND_MAX;
#endif
    return Value(r);
}

}
