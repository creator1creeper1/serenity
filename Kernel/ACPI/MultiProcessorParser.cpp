/*
 * Copyright (c) 2020, Liav A. <liavalb@hotmail.co.il>
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

#include <AK/StringView.h>
#include <Kernel/ACPI/MultiProcessorParser.h>
#include <Kernel/Arch/PC/BIOS.h>
#include <Kernel/Interrupts/IOAPIC.h>
#include <Kernel/StdLib.h>
#include <Kernel/VM/MemoryManager.h>
#include <Kernel/VM/TypedMapping.h>

//#define MULTIPROCESSOR_DEBUG

namespace Kernel {

static MultiProcessorParser* s_parser;

bool MultiProcessorParser::is_initialized()
{
    return s_parser != nullptr;
}

void MultiProcessorParser::initialize()
{
    ASSERT(!is_initialized());
    s_parser = new MultiProcessorParser;
}

MultiProcessorParser::MultiProcessorParser()
    : m_floating_pointer(search_floating_pointer())
{
    if (!m_floating_pointer.is_null()) {
        klog() << "MultiProcessor: Floating Pointer Structure @ " << PhysicalAddress(m_floating_pointer);
        parse_floating_pointer_data();
        parse_configuration_table();
    } else {
        klog() << "MultiProcessor: Can't Locate Floating Pointer Structure, disabled.";
    }
}

void MultiProcessorParser::parse_floating_pointer_data()
{
    auto floating_pointer = map_typed<MultiProcessor::FloatingPointer>(m_floating_pointer);
    m_configuration_table = PhysicalAddress(floating_pointer->physical_address_ptr);
    dbg() << "Features " << floating_pointer->feature_info[0] << ", IMCR? " << (floating_pointer->feature_info[0] & (1 << 7));
}

void MultiProcessorParser::parse_configuration_table()
{
    auto configuration_table_length = map_typed<MultiProcessor::ConfigurationTableHeader>(m_configuration_table)->length;
    auto config_table = map_typed<MultiProcessor::ConfigurationTableHeader>(m_configuration_table, configuration_table_length);

    size_t entry_count = config_table->entry_count;
    auto* entry = config_table->entries;
    while (entry_count > 0) {
#ifdef MULTIPROCESSOR_DEBUG
        dbg() << "MultiProcessor: Entry Type " << entry->entry_type << " detected.";
#endif
        switch (entry->entry_type) {
        case ((u8)MultiProcessor::ConfigurationTableEntryType::Processor):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::Processor;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::Bus):
            m_bus_entries.append(*(const MultiProcessor::BusEntry*)entry);
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::Bus;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::IOAPIC):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::IOAPIC;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::IO_Interrupt_Assignment):
            m_io_interrupt_assignment_entries.append(*(const MultiProcessor::IOInterruptAssignmentEntry*)entry);
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::IO_Interrupt_Assignment;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::Local_Interrupt_Assignment):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::Local_Interrupt_Assignment;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::SystemAddressSpaceMapping):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::SystemAddressSpaceMapping;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::BusHierarchyDescriptor):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::BusHierarchyDescriptor;
            break;
        case ((u8)MultiProcessor::ConfigurationTableEntryType::CompatibilityBusAddressSpaceModifier):
            entry = (MultiProcessor::EntryHeader*)(u32)entry + (u8)MultiProcessor::ConfigurationTableEntryLength::CompatibilityBusAddressSpaceModifier;
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        --entry_count;
    }
}

PhysicalAddress MultiProcessorParser::search_floating_pointer()
{
    auto mp_floating_pointer = search_floating_pointer_in_ebda();
    if (!mp_floating_pointer.is_null())
        return mp_floating_pointer;
    return search_floating_pointer_in_bios_area();
}

PhysicalAddress MultiProcessorParser::search_floating_pointer_in_ebda()
{
    klog() << "MultiProcessor: Probing EBDA";
    auto ebda = map_ebda();
    for (auto* floating_pointer_str = ebda.base(); floating_pointer_str < ebda.end(); floating_pointer_str += 16) {
        if (!strncmp("_MP_", (const char*)floating_pointer_str, strlen("_MP_")))
            return ebda.paddr_of(floating_pointer_str);
    }
    return {};
}
PhysicalAddress MultiProcessorParser::search_floating_pointer_in_bios_area()
{
    auto bios = map_bios();
    for (auto* floating_pointer_str = bios.base(); floating_pointer_str < bios.end(); floating_pointer_str += 16) {
        if (!strncmp("_MP_", (const char*)floating_pointer_str, strlen("_MP_")))
            return bios.paddr_of(floating_pointer_str);
    }
    return {};
}

Vector<u8> MultiProcessorParser::get_pci_bus_ids() const
{
    Vector<u8> pci_bus_ids;
    for (auto& entry : m_bus_entries) {
        if (!strncmp("PCI   ", entry.bus_type, strlen("PCI   ")))
            pci_bus_ids.append(entry.bus_id);
    }
    return pci_bus_ids;
}

MultiProcessorParser& MultiProcessorParser::the()
{
    ASSERT(is_initialized());
    return *s_parser;
}

Vector<PCIInterruptOverrideMetadata> MultiProcessorParser::get_pci_interrupt_redirections()
{
    dbg() << "MultiProcessor: Get PCI IOAPIC redirections";
    Vector<PCIInterruptOverrideMetadata> overrides;
    auto pci_bus_ids = get_pci_bus_ids();
    for (auto& entry : m_io_interrupt_assignment_entries) {
        for (auto id : pci_bus_ids) {
            if (id == entry.source_bus_id) {

                klog() << "Interrupts: Bus " << entry.source_bus_id << ", Polarity " << entry.polarity << ", Trigger Mode " << entry.trigger_mode << ", INT " << entry.source_bus_irq << ", IOAPIC " << entry.destination_ioapic_id << ", IOAPIC INTIN " << entry.destination_ioapic_intin_pin;
                overrides.empend(
                    entry.source_bus_id,
                    entry.polarity,
                    entry.trigger_mode,
                    entry.source_bus_irq,
                    entry.destination_ioapic_id,
                    entry.destination_ioapic_intin_pin);
            }
        }
    }

    for (auto& override_metadata : overrides) {
        klog() << "Interrupts: Bus " << override_metadata.bus() << ", Polarity " << override_metadata.polarity() << ", PCI Device " << override_metadata.pci_device_number() << ", Trigger Mode " << override_metadata.trigger_mode() << ", INT " << override_metadata.pci_interrupt_pin() << ", IOAPIC " << override_metadata.ioapic_id() << ", IOAPIC INTIN " << override_metadata.ioapic_interrupt_pin();
    }
    return overrides;
}

PCIInterruptOverrideMetadata::PCIInterruptOverrideMetadata(u8 bus_id, u8 polarity, u8 trigger_mode, u8 source_irq, u32 ioapic_id, u16 ioapic_int_pin)
    : m_bus_id(bus_id)
    , m_polarity(polarity)
    , m_trigger_mode(trigger_mode)
    , m_pci_interrupt_pin(source_irq & 0b11)
    , m_pci_device_number((source_irq >> 2) & 0b11111)
    , m_ioapic_id(ioapic_id)
    , m_ioapic_interrupt_pin(ioapic_int_pin)
{
}

}
