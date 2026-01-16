#pragma once

#include "file.h"

class RawFileClass : public FileClass {
public:
    explicit RawFileClass(const char* filename) : FileClass(filename) {}
    RawFileClass() : FileClass(NULL) {}

    virtual char const* Set_Name(char const* filename);
    virtual char const* File_Name() const;
    virtual int Open(int rights = READ);
    virtual int Open(char const* filename, int rights = READ);
    virtual int Is_Open() const;
    virtual int Is_Available(int forced = false);
    virtual long Seek(long pos, int dir = SEEK_CUR);
};

