#include "anim.h"

#include <new>

void* AnimClass::VTable = NULL;

void* AnimClass::operator new(size_t size) { return ::operator new(size); }

void AnimClass::operator delete(void* ptr) { ::operator delete(ptr); }

AnimClass::AnimClass(void)
    : Object(NULL),
      Owner(HOUSE_NONE),
      Loops(0),
      IsToDelete(0),
      IsBrandNew(1),
      IsAlternate(0),
      IsInvisible(0),
      Class(NULL),
      Delay(0),
      Accum(0) {}

AnimClass::AnimClass(AnimType animnum, COORDINATE coord, unsigned char timedelay, unsigned char loop, bool alt)
    : Object(NULL),
      Owner(HOUSE_NONE),
      Loops(loop),
      IsToDelete(0),
      IsBrandNew(1),
      IsAlternate(alt ? 1U : 0U),
      IsInvisible(0),
      Class(&AnimTypeClass::As_Reference(animnum)),
      Delay(timedelay),
      Accum(0) {
    Coord = coord;
}

AnimClass::~AnimClass(void) {}

AnimClass::operator AnimType(void) const { return Class ? Class->Type : ANIM_NONE; }

void AnimClass::Init(void) {
    // Placeholder: original resets a fixed heap pool and captures the VTable.
}

void AnimClass::Attach_To(ObjectClass* obj) { Object = obj; }

bool AnimClass::Mark(MarkType) { return true; }

bool AnimClass::Render(bool forced) {
    (void)forced;
    if (Delay) return false;
    if (IsInvisible) return false;
    return true;
}

COORDINATE AnimClass::Center_Coord(void) const { return Coord; }

COORDINATE AnimClass::Sort_Y(void) const { return Center_Coord(); }

LayerType AnimClass::In_Which_Layer(void) const {
    if (Class && Class->IsGroundLayer) return LAYER_GROUND;
    return LAYER_AIR;
}

ObjectTypeClass const& AnimClass::Class_Of(void) const { return Class ? *Class : AnimTypeClass::As_Reference(ANIM_FIRST); }

short const* AnimClass::Occupy_List(void) const {
    static short const kNone[] = {REFRESH_EOL};
    return kNone;
}

short const* AnimClass::Overlap_List(void) const {
    static short const kNone[] = {REFRESH_EOL};
    return kNone;
}

void AnimClass::Draw_It(int, int, WindowNumberType) {}

void AnimClass::AI(void) {
    if (Delay) {
        --Delay;
        return;
    }
    (void)Graphic_Logic();
}

TARGET AnimClass::As_Target(void) const { return 0; }

void AnimClass::Detach(TARGET, bool) {}

bool AnimClass::Load(FileClass&) { return true; }

bool AnimClass::Save(FileClass&) { return true; }

void AnimClass::Code_Pointers(void) {}

void AnimClass::Decode_Pointers(void) {}

int AnimClass::Validate(void) const { return 1; }

void AnimClass::Middle(void) {}

void AnimClass::Start(void) {}

COORDINATE AnimClass::Adjust_Coord(COORDINATE coord) { return coord; }
