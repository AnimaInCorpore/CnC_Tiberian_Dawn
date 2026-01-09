#pragma once

#include "legacy_compat.h"

class CDFileClass : public RawFileClass {
public:
    explicit CDFileClass(char const* filename);
    CDFileClass();
    virtual ~CDFileClass() {}

    virtual char const* Set_Name(char const* filename);
    virtual int Open(char const* filename, int rights = READ);
    virtual int Open(int rights = READ);

    void Searching(int on) { IsDisabled = !on; }

    static bool Is_There_Search_Drives();
    static int Set_Search_Drives(char* pathlist);
    static void Add_Search_Drive(char* path);
    static void Clear_Search_Drives();
    static void Refresh_Search_Drives();
    static void Set_CD_Drive(int drive);
    static int Get_CD_Drive();
    static int Get_Last_CD_Drive();

private:
    bool IsDisabled;
};

