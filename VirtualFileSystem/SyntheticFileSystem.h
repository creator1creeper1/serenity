#pragma once

#include "FileSystem.h"
#include "UnixTypes.h"
#include <AK/HashMap.h>

class SynthFSInode;

class SynthFS : public FS {
public:
    virtual ~SynthFS() override;
    static RetainPtr<SynthFS> create();

    virtual bool initialize() override;
    virtual const char* class_name() const override;
    virtual InodeIdentifier root_inode() const override;
    virtual RetainPtr<Inode> create_inode(InodeIdentifier parentInode, const String& name, Unix::mode_t, unsigned size, int& error) override;
    virtual RetainPtr<Inode> create_directory(InodeIdentifier parentInode, const String& name, Unix::mode_t, int& error) override;
    virtual RetainPtr<Inode> get_inode(InodeIdentifier) const override;

protected:
    typedef unsigned InodeIndex;

    InodeIndex generate_inode_index();
    static constexpr InodeIndex RootInodeIndex = 1;

    SynthFS();

    RetainPtr<SynthFSInode> create_directory(String&& name);
    RetainPtr<SynthFSInode> create_text_file(String&& name, ByteBuffer&&, Unix::mode_t = 0010644);
    RetainPtr<SynthFSInode> create_generated_file(String&& name, Function<ByteBuffer(SynthFSInode&)>&&, Unix::mode_t = 0100644);
    RetainPtr<SynthFSInode> create_generated_file(String&& name, Function<ByteBuffer(SynthFSInode&)>&&, Function<ssize_t(SynthFSInode&, const ByteBuffer&)>&&, Unix::mode_t = 0100644);

    InodeIdentifier add_file(RetainPtr<SynthFSInode>&&, InodeIndex parent = RootInodeIndex);
    bool remove_file(InodeIndex);

private:
    InodeIndex m_next_inode_index { 2 };
    HashMap<InodeIndex, RetainPtr<SynthFSInode>> m_inodes;
};

struct SynthFSInodeCustomData {
    virtual ~SynthFSInodeCustomData();
};

class SynthFSInode final : public Inode {
    friend class SynthFS;
public:
    virtual ~SynthFSInode() override;

    void set_custom_data(OwnPtr<SynthFSInodeCustomData>&& custom_data) { m_custom_data = move(custom_data); }
    SynthFSInodeCustomData* custom_data() { return m_custom_data.ptr(); }
    const SynthFSInodeCustomData* custom_data() const { return m_custom_data.ptr(); }

private:
    // ^Inode
    virtual ssize_t read_bytes(Unix::off_t, size_t, byte* buffer, FileDescriptor*) override;
    virtual InodeMetadata metadata() const override;
    virtual bool traverse_as_directory(Function<bool(const FS::DirectoryEntry&)>) override;
    virtual InodeIdentifier lookup(const String& name) override;
    virtual String reverse_lookup(InodeIdentifier) override;
    virtual void flush_metadata() override;
    virtual bool write(const ByteBuffer&) override;
    virtual bool add_child(InodeIdentifier child_id, const String& name, byte file_type, int& error) override;
    virtual RetainPtr<Inode> parent() const override;

    SynthFS& fs();
    const SynthFS& fs() const;
    SynthFSInode(SynthFS&, unsigned index);

    String m_name;
    InodeIdentifier m_parent;
    ByteBuffer m_data;
    Function<ByteBuffer(SynthFSInode&)> m_generator;
    Function<ssize_t(SynthFSInode&, const ByteBuffer&)> m_write_callback;
    Vector<SynthFSInode*> m_children;
    InodeMetadata m_metadata;
    OwnPtr<SynthFSInodeCustomData> m_custom_data;
};

inline SynthFS& SynthFSInode::fs()
{
    return static_cast<SynthFS&>(Inode::fs());
}

inline const SynthFS& SynthFSInode::fs() const
{
    return static_cast<const SynthFS&>(Inode::fs());
}
