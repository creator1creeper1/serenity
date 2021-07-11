/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Heap/kmalloc.h>
#include <Kernel/VM/MemoryManager.h>
#include <Kernel/VM/PhysicalPage.h>

namespace Kernel {

NonnullRefPtr<PhysicalPage> PhysicalPage::create(PhysicalAddress paddr, bool may_return_to_freelist)
{
    auto& physical_page_entry = MM.get_physical_page_entry(paddr);
    return adopt_ref(*new (&physical_page_entry.physical_page) PhysicalPage(may_return_to_freelist));
}

PhysicalPage::PhysicalPage(bool may_return_to_freelist)
    : m_may_return_to_freelist(may_return_to_freelist)
{
}

PhysicalAddress PhysicalPage::paddr() const
{
    return MM.get_physical_address(*this);
}

void PhysicalPage::free_this()
{
    if (m_may_return_to_freelist) {
        auto paddr = MM.get_physical_address(*this);
        this->~PhysicalPage(); // delete in place
        MM.deallocate_physical_page(paddr);
    } else {
        this->~PhysicalPage(); // delete in place
    }
}

}
