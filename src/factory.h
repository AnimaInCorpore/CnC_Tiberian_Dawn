#pragma once

#include <cstddef>

#include "stage.h"

class FileClass;
class HouseClass;
class TechnoClass;
class TechnoTypeClass;

class FactoryClass : private StageClass {
public:
    FactoryClass(void);
    ~FactoryClass(void);
    static void* operator new(size_t size);
    static void operator delete(void* ptr);

    static void Init(void);

    /*
    **  File I/O (implemented in ioobj.cpp).
    */
    bool Load(FileClass& file);
    bool Save(FileClass& file);
    void Code_Pointers(void);
    void Decode_Pointers(void);

    bool Abandon(void);
    bool Completed(void);
    bool Has_Changed(void);
    bool Has_Completed(void);
    bool Is_Building(void) const { return (Fetch_Rate() != 0); }
    bool Set(TechnoTypeClass const& object, HouseClass& house);
    bool Set(int const& type, HouseClass& house);
    bool Start(void);
    bool Suspend(void);
    int Completion(void);
    TechnoClass* Get_Object(void) const;
    int Get_Special_Item(void) const;
    void AI(void);
    void Set(TechnoClass& object);
    HouseClass* Get_House(void) { return House; }

    /*
    **  Dee-buggin' support.
    */
    int Validate(void) const;

    /*
    **  This flag is used to maintain the pool of factory class objects. If the object has
    **  been allocated, then this flag is true. Otherwise, the object is free to be
    **  allocated.
    */
    unsigned IsActive : 1;

protected:
    enum StepCountEnum { STEP_COUNT = 108 };

    int Cost_Per_Tick(void);

private:
    unsigned IsSuspended : 1;
    unsigned IsDifferent : 1;

    int Balance;
    int OriginalBalance;

    TechnoClass* Object;
    int SpecialItem;
    HouseClass* House;
};

