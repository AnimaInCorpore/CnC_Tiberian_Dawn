#pragma once

#include "legacy_compat.h"
#include "facing.h"

class TechnoClass;

class BulletClass : public ObjectClass, public FlyClass, public FuseClass {
public:
    static void* VTable;

    static void* operator new(std::size_t size);
    static void operator delete(void* ptr);

    explicit BulletClass(BulletType type = BULLET_FIRST);
    virtual ~BulletClass();

    virtual RTTIType What_Am_I(void) const { return RTTI_BULLET; }
    operator BulletType(void) const { return Class ? Class->Type : BULLET_NONE; }

    static void Init(void);

    virtual void Assign_Target(TARGET target) { TarCom = target; }
    virtual bool Unlimbo(COORDINATE, DirType = 0) { return true; }
    virtual LayerType In_Which_Layer(void) const { return LAYER_TOP; }
    virtual ObjectTypeClass const& Class_Of(void) const { return *Class; }
    virtual void Detach(TARGET, bool) {}
    virtual void Draw_It(int, int, WindowNumberType) {}
    virtual bool Mark(MarkType = MARK_CHANGE) { return true; }
    virtual void AI(void) {}
    virtual short const* Occupy_List(void) const;
    virtual TARGET As_Target(void) const { return 0; }

    bool Load(FileClass&) { return true; }
    bool Save(FileClass&) { return true; }
    virtual void Code_Pointers(void) {}
    virtual void Decode_Pointers(void) {}
    int Validate(void) const { return 1; }

    BulletTypeClass const* const Class;
    TechnoClass* Payback;
    FacingClass PrimaryFacing;
    unsigned IsInaccurate : 1;

private:
    unsigned IsToAnimate : 1;
    int Altitude;
    signed char Riser;
    TARGET TarCom;
    unsigned IsLocked : 1;
};
