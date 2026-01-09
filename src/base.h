#pragma once

#include "legacy_compat.h"

class BaseNodeClass {
public:
    BaseNodeClass(void) : Type(STRUCT_NONE), Coord(0) {}
    int operator==(BaseNodeClass const& node);
    int operator!=(BaseNodeClass const& node);
    int operator>(BaseNodeClass const& node);

    StructType Type;
    COORDINATE Coord;
};

class BaseClass {
public:
    BaseClass(void) : House(HOUSE_NONE) {}
    virtual ~BaseClass() { Nodes.Clear(); }

    void Init(void) { Nodes.Clear(); }

    void Read_INI(char* buffer);
    void Write_INI(char* buffer);
    static char* INI_Name(void) { return (char*)"Base"; }
    bool Load(FileClass& file);
    bool Save(FileClass& file);
    virtual void Code_Pointers(void) {}
    virtual void Decode_Pointers(void) {}

    bool Is_Built(int index);
    BuildingClass* Get_Building(int index);
    bool Is_Node(BuildingClass* obj);
    BaseNodeClass* Get_Node(BuildingClass* obj);
    BaseNodeClass* Get_Node(int index) { return (&Nodes[index]); }
    BaseNodeClass* Next_Buildable(StructType type = STRUCT_NONE);

    DynamicVectorClass<BaseNodeClass> Nodes;
    HousesType House;
};

