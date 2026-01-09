#include "bullet.h"

#include <new>

void* BulletClass::VTable = NULL;

static BulletType Sanitize_BulletType(BulletType type) {
    if (type < BULLET_FIRST || type >= BULLET_COUNT) return BULLET_FIRST;
    return type;
}

void* BulletClass::operator new(std::size_t size) { return ::operator new(size); }

void BulletClass::operator delete(void* ptr) { ::operator delete(ptr); }

BulletClass::BulletClass(BulletType type)
    : Class(&BulletTypeClass::As_Reference(Sanitize_BulletType(type))),
      Payback(NULL),
      PrimaryFacing(0),
      IsInaccurate(0),
      IsToAnimate(0),
      Altitude(0),
      Riser(0),
      TarCom(0),
      IsLocked(1) {}

BulletClass::~BulletClass() {}

void BulletClass::Init(void) {
    // Placeholder: original resets the bullet pool and captures the VTable.
}

short const* BulletClass::Occupy_List(void) const {
    static short const kNone[] = {REFRESH_EOL};
    return kNone;
}

