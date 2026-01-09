#pragma once

#include "legacy_compat.h"

class TemplateTypeClass : public ObjectTypeClass {
public:
    TemplateTypeClass(TemplateType iconset,
                      int theater,
                      char const* ininame,
                      int fullname,
                      LandType land,
                      int width,
                      int height,
                      LandType altland,
                      char const* alticons);

    static TemplateTypeClass const& As_Reference(TemplateType type) { return *Pointers[static_cast<int>(type)]; }
    static TemplateType From_Name(char const* name);
    static void Init(TheaterType theater);
    static void One_Time(void);

    virtual short const* Occupy_List(bool placement = false) const;
#ifdef SCENARIO_EDITOR
    virtual void Display(int x, int y, WindowNumberType window, HousesType house) const;
    static void Prep_For_Add(void);
#endif

    virtual bool Create_And_Place(CELL cell, HousesType house) const;
    virtual ObjectClass* Create_One_Of(HouseClass* house) const;

    int Theater;
    char const* AltIcons;
    LandType AltLand;
    TemplateType Type;
    LandType Land;
    int Width;
    int Height;

    static TemplateTypeClass const* const Pointers[TEMPLATE_COUNT];
};

