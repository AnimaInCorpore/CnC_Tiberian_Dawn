#pragma once

#include <string>
#include <cstdio>
#include <cstddef>

#ifndef READ
#define READ 1
#endif

#ifndef WRITE
#define WRITE 2
#endif

class FileClass {
public:
    explicit FileClass(const char* filename);
    virtual ~FileClass();

    virtual bool Is_Available();
    virtual long Size();
    virtual long Read(void* buffer, long length);
    virtual long Write(void const* buffer, long length);
    virtual void Close();

protected:
    std::string Filename;
    std::FILE* Handle;
};

// Global helper to load file content into a new buffer.
// Returns a pointer to the buffer (caller must delete[]) or NULL on failure.
void* Load_Alloc_Data(FileClass& file);
