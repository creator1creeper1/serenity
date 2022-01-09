/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Bitmap.h>
#include <Kernel/Memory/VMObject.h>
#include <Kernel/UnixTypes.h>

namespace Kernel::Memory {

class InodeVMObject : public VMObject {
public:
    virtual ~InodeVMObject() override;

    Inode& inode() { return *m_inode; }
    Inode const& inode() const { return *m_inode; }

    size_t amount_dirty() const;
    size_t amount_clean() const;

    int release_all_clean_pages();

    u32 writable_mappings() const;
    u32 executable_mappings() const;

protected:
    static ErrorOr<InodeVMObject> try_create(Inode& inode, size_t size);
    ErrorOr<InodeVMObject> try_clone_nonvirtual();

    InodeVMObject& operator=(InodeVMObject const&) = delete;
    InodeVMObject& operator=(InodeVMObject&&) = delete;
    InodeVMObject(InodeVMObject&&);

    virtual bool is_inode() const final { return true; }

    NonnullRefPtr<Inode> m_inode;
    Bitmap m_dirty_pages;

private:
    explicit InodeVMObject(Inode&, size_t);
    explicit InodeVMObject(InodeVMObject const&);
};

}
