/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Singleton.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/VMObject.h>

namespace Kernel::Memory {

static Singleton<SpinlockProtected<VMObject::AllInstancesList>> s_all_instances;

SpinlockProtected<VMObject::AllInstancesList>& VMObject::all_instances()
{
    return s_all_instances;
}

ErrorOr<void> VMObject::initialize(VMObject const& other)
{
    TRY(initialize_common());
    new (&m_physical_pages) FixedArray<RefPtr<PhysicalPage>>(TRY(other.m_physical_pages.try_clone()));
    all_instances().with([&](auto& list) { list.append(*this); });
}

ErrorOr<void> VMObject::initialize(size_t size)
{
    TRY(initialize_common());
    new (&m_physical_pages) FixedArray<RefPtr<PhysicalPage>>(TRY(FixedArray<RefPtr<PhysicalPage>>::try_create(ceil_div(size, static_cast<size_t>(PAGE_SIZE)))));
    all_instances().with([&](auto& list) { list.append(*this); });
}

ErrorOr<void> VMObject::initialize_common()
{
    TRY((static_cast<InheritedInitializable<VMObject, ListedRefCountedSpinlock> *>(this))->initialize());
    TRY((static_cast<InheritedInitializable<VMObject, Weakable> *>(this))->initialize());
    new (&m_list_node) IntrusiveListNode<VMObject>;
    new (&m_lock) RecursiveSpinlock;
    new (&m_regions) Region::ListInVMObject;
}

VMObject::~VMObject()
{
    VERIFY(m_regions.is_empty());
}

}
