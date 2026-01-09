#pragma once

#include "legacy_compat.h"
#include "stage.h"

class AnimClass : public ObjectClass, private StageClass {
public:
    static void* operator new(size_t size);
    static void operator delete(void* ptr);

    AnimClass(void);
    AnimClass(AnimType animnum, COORDINATE coord, unsigned char timedelay = 0, unsigned char loop = 1, bool alt = false);
    virtual ~AnimClass(void);

    operator AnimType(void) const;
    virtual RTTIType What_Am_I(void) const { return RTTI_ANIM; }

    static void Init(void);

    void Attach_To(ObjectClass* obj);
    void Make_Invisible(void) { IsInvisible = true; }

    virtual bool Can_Place_Here(COORDINATE) const { return true; }
    virtual bool Mark(MarkType mark = MARK_CHANGE);
    virtual bool Render(bool forced);
    virtual COORDINATE Center_Coord(void) const;
    virtual COORDINATE Sort_Y(void) const;
    virtual LayerType In_Which_Layer(void) const;
    virtual ObjectTypeClass const& Class_Of(void) const;
    virtual short const* Occupy_List(void) const;
    virtual short const* Overlap_List(void) const;
    virtual void Draw_It(int x, int y, WindowNumberType window);
    virtual void AI(void);
    virtual TARGET As_Target(void) const;
    virtual void Detach(TARGET target, bool all);

    bool Load(FileClass& file);
    bool Save(FileClass& file);
    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

    int Validate(void) const;

    ObjectClass* Object;
    HousesType Owner;
    unsigned char Loops;

protected:
    void Middle(void);
    void Start(void);

private:
    COORDINATE Adjust_Coord(COORDINATE coord);

    unsigned IsToDelete : 1;
    unsigned IsBrandNew : 1;
    unsigned IsAlternate : 1;
    unsigned IsInvisible : 1;

    AnimTypeClass const* const Class;
    unsigned char Delay;
    unsigned char Accum;

    static void* VTable;
};
