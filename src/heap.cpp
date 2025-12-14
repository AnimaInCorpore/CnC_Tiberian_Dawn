#include "legacy/heap.h"

#include <cstring>
#include <new>

#include "legacy/defines.h"
#include "legacy/cell.h"
#include "legacy/function.h"
#include "legacy/aircraft.h"
#include "legacy/anim.h"
#include "legacy/bullet.h"
#include "legacy/building.h"
#include "legacy/factory.h"
#include "legacy/house.h"
#include "legacy/infantry.h"
#include "legacy/overlay.h"
#include "legacy/smudge.h"
#include "legacy/team.h"
#include "legacy/teamtype.h"
#include "legacy/template.h"
#include "legacy/terrain.h"
#include "legacy/trigger.h"
#include "legacy/unit.h"
#include "legacy/wwfile.h"

#include "legacy/heap.h"

#include <cstring>
#include <new>

#include "legacy/defines.h"
#include "legacy/cell.h"
#include "legacy/function.h"
#include "legacy/aircraft.h"
#include "legacy/anim.h"
#include "legacy/bullet.h"
#include "legacy/building.h"
#include "legacy/factory.h"
#include "legacy/house.h"
#include "legacy/infantry.h"
#include "legacy/overlay.h"
#include "legacy/smudge.h"
#include "legacy/team.h"
#include "legacy/teamtype.h"
#include "legacy/template.h"
#include "legacy/terrain.h"
#include "legacy/trigger.h"
#include "legacy/unit.h"
#include "legacy/wwfile.h"

FixedHeapClass::FixedHeapClass(int size)
    : IsAllocated(false),
      Size(size),
      TotalCount(0),
      ActiveCount(0),
      Buffer(nullptr) {}

FixedHeapClass::~FixedHeapClass(void) { FixedHeapClass::Clear(); }

int FixedHeapClass::Set_Heap(int count, void* buffer) {
  Clear();

  if (!Size) {
    return false;
  }

  if (!count) {
    return true;
  }

  if (FreeFlag.Resize(count)) {
    if (!buffer) {
      buffer = new (std::nothrow) char[count * Size];
      if (!buffer) {
        FreeFlag.Clear();
        return false;
      }
      IsAllocated = true;
    }
    Buffer = buffer;
    TotalCount = count;
    return true;
  }
  return false;
}

void* FixedHeapClass::Allocate(void) {
  if (ActiveCount < TotalCount) {
    int index = FreeFlag.First_False();

    if (index != -1) {
      ActiveCount++;
      FreeFlag[index] = true;
      return (*this)[index];
    }
  }
  return nullptr;
}

int FixedHeapClass::Free(void* pointer) {
  if (pointer && ActiveCount) {
    int index = ID(pointer);

    if ((unsigned)index < (unsigned)TotalCount) {
      if (FreeFlag[index]) {
        ActiveCount--;
        FreeFlag[index] = false;
        return true;
      }
    }
  }
  return false;
}

int FixedHeapClass::ID(void const* pointer) {
  if (pointer && Size) {
    return (int)(((char*)pointer - (char*)Buffer) / Size);
  }
  return -1;
}

void FixedHeapClass::Clear(void) {
  if (Buffer && IsAllocated) {
    delete[] static_cast<char*>(Buffer);
  }
  Buffer = nullptr;
  IsAllocated = false;
  ActiveCount = 0;
  TotalCount = 0;
  FreeFlag.Clear();
}

int FixedHeapClass::Free_All(void) {
  ActiveCount = 0;
  FreeFlag.Reset();
  return true;
}

int FixedIHeapClass::Free_All(void) {
  ActivePointers.Delete_All();
  return FixedHeapClass::Free_All();
}

void FixedIHeapClass::Clear(void) {
  FixedHeapClass::Clear();
  ActivePointers.Clear();
}

int FixedIHeapClass::Set_Heap(int count, void* buffer) {
  Clear();
  if (FixedHeapClass::Set_Heap(count, buffer)) {
    ActivePointers.Resize(count);
    return true;
  }
  return false;
}

void* FixedIHeapClass::Allocate(void) {
  void* ptr = FixedHeapClass::Allocate();
  if (ptr) {
    ActivePointers.Add(ptr);
    std::memset(ptr, 0, Size);
  }
  return ptr;
}

int FixedIHeapClass::Free(void* pointer) {
  if (FixedHeapClass::Free(pointer)) {
    ActivePointers.Delete(pointer);
  }
  return false;
}

FixedIHeapClass::~FixedIHeapClass(void) = default;

template <class T>
int TFixedIHeapClass<T>::Save(FileClass& file) {
  // Stubbed persistence for porting: just acknowledge the write.
  (void)file;
  return true;
}

template <class T>
int TFixedIHeapClass<T>::Load(FileClass& file) {
  // Stubbed load for porting: do nothing and report success.
  (void)file;
  return true;
}

template <class T>
void TFixedIHeapClass<T>::Code_Pointers(void) {
  // Stubbed no-op.
}

template <class T>
void TFixedIHeapClass<T>::Decode_Pointers(void) {
  // Stubbed no-op.
}

template class TFixedIHeapClass<UnitClass>;
template class TFixedIHeapClass<FactoryClass>;
template class TFixedIHeapClass<TerrainClass>;
template class TFixedIHeapClass<TemplateClass>;
template class TFixedIHeapClass<SmudgeClass>;
template class TFixedIHeapClass<OverlayClass>;
template class TFixedIHeapClass<InfantryClass>;
template class TFixedIHeapClass<BulletClass>;
template class TFixedIHeapClass<BuildingClass>;
template class TFixedIHeapClass<AnimClass>;
template class TFixedIHeapClass<AircraftClass>;
template class TFixedIHeapClass<TriggerClass>;
template class TFixedIHeapClass<TeamTypeClass>;
template class TFixedIHeapClass<TeamClass>;
template class TFixedIHeapClass<HouseClass>;
