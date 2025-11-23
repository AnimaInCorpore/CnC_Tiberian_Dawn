#pragma once

#include "function.h"
#include "type.h"

class AbstractClass
{
public:
    COORDINATE Coord;
    unsigned IsActive:1;

    AbstractClass(void) {Coord = 0L;};
    virtual ~AbstractClass(void) {};

    virtual HousesType Owner(void) const {return HOUSE_NONE;};
    virtual COORDINATE Center_Coord(void) const {return Coord;};
    virtual COORDINATE Target_Coord(void) const {return Coord;};

    DirType Direction(AbstractClass const * object) const {return ::Direction(Center_Coord(), object->Target_Coord());};
    DirType Direction(COORDINATE coord) const {return ::Direction(Center_Coord(), coord);};
    DirType Direction(TARGET target) const {return ::Direction(Center_Coord(), As_Coord(target));};
    DirType Direction(CELL cell) const {return ::Direction(Coord_Cell(Center_Coord()), cell);};
    int Distance(TARGET target) const;
    int Distance(COORDINATE coord) const {return ::Distance(Center_Coord(), coord);};
    int Distance(CELL cell) const {return ::Distance(Coord_Cell(Center_Coord()), cell);};
    int Distance(AbstractClass const * object) const {return ::Distance(Center_Coord(), object->Target_Coord());};

    virtual MoveType Can_Enter_Cell(CELL , FacingType = FACING_NONE) const {return MOVE_OK;};
    virtual void AI(void) {};
};

inline RTTIType AbstractTypeClass::What_Am_I(void) const {return RTTI_ABSTRACTTYPE;};
inline COORDINATE AbstractTypeClass::Coord_Fixup(COORDINATE coord) const {return coord;}
inline int AbstractTypeClass::Full_Name(void) const {return Name;};
inline unsigned short AbstractTypeClass::Get_Ownable(void) const {return 0xffff;};
