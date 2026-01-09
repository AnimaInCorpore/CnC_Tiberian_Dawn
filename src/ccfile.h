#pragma once

#include "cdfile.h"

#include <cerrno>

class CCFileClass : public CDFileClass {
public:
    CCFileClass(char const* filename);
    CCFileClass();
    virtual ~CCFileClass() {}

    virtual int Open(char const* filename, int rights = READ) {
        Set_Name(filename);
        return Open(rights);
    }
    virtual int Open(int rights = READ);
    virtual int Is_Open() const;
    virtual int Is_Available(int forced = false);
    virtual long Read(void* buffer, long size);
    virtual long Seek(long pos, int dir = SEEK_CUR);
    virtual long Size();
    virtual long Write(void const* buffer, long size);
    virtual void Close();
    virtual void Error(int error, int canretry = false, char const* filename = NULL);

private:
    bool FromDisk;
    void* Pointer;
    long Start;
    long Position;
    long Length;

    CCFileClass const operator=(CCFileClass const&);
    CCFileClass(CCFileClass const&);
};
