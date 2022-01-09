/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Memory/PrivateInodeVMObject.h>

namespace Kernel::Memory {

ErrorOr<NonnullRefPtr<PrivateInodeVMObject>> PrivateInodeVMObject::try_create_with_inode(Inode& inode)
{
    InodeVMObject new_parent = TRY(InodeVMObject::try_create(inode, inode.size()));
    return adopt_nonnull_ref_or_enomem(new (nothrow) PrivateInodeVMObject(move(new_parent)));
}

ErrorOr<NonnullRefPtr<VMObject>> PrivateInodeVMObject::try_clone()
{
    InodeVMObject new_parent = TRY(static_cast<InodeVMObject*>(this)->try_clone_nonvirtual());
    return adopt_nonnull_ref_or_enomem<VMObject>(new (nothrow) PrivateInodeVMObject(move(new_parent)));
}

PrivateInodeVMObject::PrivateInodeVMObject(InodeVMObject&& new_parent)
    : InodeVMObject(move(new_parent))
{
}

PrivateInodeVMObject::~PrivateInodeVMObject()
{
}

}
