#pragma once

#include "cell.h"

class ObjectTypeClass;

class MapClass {
public:
    CellClass & operator[](CELL cell);
    CellClass const & operator[](CELL cell) const;

    bool In_Radar(CELL cell) const;
    bool Cell_Template(CELL cell) const;
    void Add_To_List(ObjectTypeClass * obj);
};

extern MapClass Map;
