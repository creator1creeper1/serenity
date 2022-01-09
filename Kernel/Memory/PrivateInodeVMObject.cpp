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
    NonnullRefPtr<PrivateInoveVMObject> new_this = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) PrivateInodeVMObject()));
    TRY(InodeVMObject::try_create_into_child(inode, inode.size(), *new_this));
    return new_this;
}

ErrorOr<NonnullRefPtr<VMObject>> PrivateInodeVMObject::try_clone()
{
    NonnullRefPtr<PrivateInodeVMObject> new_this = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) PrivateInodeVMObject()));
    TRY(static_cast<InodeVMObject*>(this)->try_clone_into_child(*new_this));
    return new_this;
}

PrivateInodeVMObject::PrivateInodeVMObject()
    : InodeVMObject()
{
}

PrivateInodeVMObject::~PrivateInodeVMObject()
{
}

}
