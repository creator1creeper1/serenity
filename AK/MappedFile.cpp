#include "MappedFile.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>

namespace AK {

MappedFile::MappedFile(String&& fileName)
    : m_fileName(std::move(fileName))
{
    m_fileLength = 1024;
    m_fd = open(m_fileName.characters(), O_RDONLY);
    
    if (m_fd != -1) {
        struct stat st;
        fstat(m_fd, &st);
        m_fileLength = st.st_size;
        m_map = mmap(nullptr, m_fileLength, PROT_READ, MAP_SHARED, m_fd, 0);

        if (m_map == MAP_FAILED)
            perror("");
    }

    printf("MappedFile{%s} := { m_fd=%d, m_fileLength=%zu, m_map=%p }\n", m_fileName.characters(), m_fd, m_fileLength, m_map);
}

MappedFile::~MappedFile()
{
    if (m_map != (void*)-1) {
        ASSERT(m_fd != -1);
        munmap(m_map, m_fileLength);
    }
}

MappedFile::MappedFile(MappedFile&& other)
    : m_fileName(std::move(other.m_fileName))
    , m_fileLength(other.m_fileLength)
    , m_fd(other.m_fd)
    , m_map(other.m_map)
{
    other.m_fileLength = 0;
    other.m_fd = -1;
    other.m_map = (void*)-1;
}

}

