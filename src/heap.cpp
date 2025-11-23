#include "legacy/function.h"

#include <cstring>
#include <limits>

/*
**  Port of the legacy heap manager used for iteratable collections.
**  This brings over the FixedHeapClass/FixedIHeapClass helpers along with
**  the templated TFixedIHeapClass methods required by GLOBALS.cpp.
*/

int FixedHeapClass::ID(void const* pointer) {
  if (pointer && Size) {
    return (int)(((char*)pointer - (char*)Buffer) / Size);
  }
  return -1;
}

void FixedHeapClass::Clear(void) {
  if (Buffer && IsAllocated) {
    delete[] Buffer;
  }
  Buffer = 0;
  IsAllocated = false;
  ActiveCount = 0;
  TotalCount = 0;
  FreeFlag.Reset();
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

int FixedHeapClass::Set_Heap(int size, void* heap) {
  if (Size == size && Buffer == heap) {
    return true;
  }
  Clear();
  Size = size;
  if (!size) {
    return true;
  }
  if (heap) {
    Buffer = heap;
    IsAllocated = false;
    TotalCount = 65535 / Size;
  } else {
    Buffer = new char[Size * TotalCount];
    IsAllocated = true;
  }
  FreeFlag.Resize(TotalCount);
  return true;
}

FixedHeapClass::FixedHeapClass(int size)
    : Buffer(0), Size(size), ActiveCount(0), TotalCount(0), IsAllocated(false) {}

FixedHeapClass::~FixedHeapClass(void) { Clear(); }

void* FixedHeapClass::Allocate(void const* data) {
  if (!TotalCount) {
    return nullptr;
  }

  int index = FreeFlag.First_False();
  if (index == -1) {
    if (TotalCount < FreeFlag.Length()) {
      TotalCount = FreeFlag.Length();
    }
    if (TotalCount >= 65535) {
      return nullptr;
    }
    TotalCount += GrowthStep;
    FreeFlag.Resize(TotalCount);
    index = FreeFlag.First_False();
    if (index == -1) {
      return nullptr;
    }
  }

  FreeFlag[index] = true;
  void* ptr = (void*)((char*)Buffer + (index * Size));

  if (data) {
    std::memcpy(ptr, data, Size);
  }
  ActiveCount++;
  return ptr;
}

int FixedHeapClass::Free(void* pointer) {
  if (!pointer) {
    return false;
  }
  int index = ID(pointer);
  if (index >= 0 && index < FreeFlag.Length() && FreeFlag.Is_True(index)) {
    FreeFlag[index] = false;
    ActiveCount--;
    return true;
  }
  return false;
}

void FixedIHeapClass::Init(int size, int grow) {
  FixedHeapClass::Init(size, grow);
  ActivePointers.Resize(Length());
}

FixedIHeapClass::FixedIHeapClass(int size)
    : FixedHeapClass(size), ActivePointers(0) {}

FixedIHeapClass::~FixedIHeapClass(void) { ActivePointers.Clear(); }

void FixedHeapClass::Init(int size, int grow) {
  Clear();
  Size = size;
  GrowthStep = grow;
}

void* FixedIHeapClass::Allocate(void const* data) {
  void* ptr = FixedHeapClass::Allocate(data);
  if (ptr) {
    ActivePointers.Add(ptr);
  }
  return ptr;
}

int FixedIHeapClass::Free(void* pointer) {
  if (!FixedHeapClass::Free(pointer)) {
    return false;
  }

  int index = ActivePointers.ID(pointer);
  if (index != -1) {
    ActivePointers.Delete(index);
  }
  return true;
}

int FixedIHeapClass::Save(FileClass& file) {
  int active_count = ActivePointers.Count();
  if (!file.Write(&active_count, sizeof(active_count))) {
    return false;
  }

  for (int index = 0; index < active_count; ++index) {
    void* ptr = ActivePointers[index];
    if (!file.Write(ptr, Size)) {
      return false;
    }
  }
  return true;
}

int FixedIHeapClass::Load(FileClass& file) {
  Clear();

  int active_count = 0;
  if (!file.Read(&active_count, sizeof(active_count))) {
    return false;
  }

  for (int index = 0; index < active_count; ++index) {
    void* ptr = Allocate();
    if (!ptr) {
      return false;
    }
    if (!file.Read(ptr, Size)) {
      return false;
    }
  }
  return true;
}

void FixedIHeapClass::Code_Pointers(void) {
  for (int index = 0; index < ActivePointers.Count(); ++index) {
    ActivePointers[index] =
        reinterpret_cast<void*>(reinterpret_cast<TARGET>(ActivePointers[index]));
  }
}

void FixedIHeapClass::Decode_Pointers(void) {
  for (int index = 0; index < ActivePointers.Count(); ++index) {
    ActivePointers[index] =
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(
            ActivePointers[index]) &
        std::numeric_limits<TARGET>::max());
  }
}

  if (!pointer) {
    return false;
  }
  int index = ActivePointers.ID(pointer);
  if (index == -1) {
    return false;
  }
  ActivePointers.Delete(index);
  return FixedHeapClass::Free(pointer);
}

template <class T>
int TFixedIHeapClass<T>::Save(FileClass& file) {
  int active_count = ActivePointers.Count();
  if (!file.Write(&active_count, sizeof(active_count))) {
    return false;
  }
  for (int index = 0; index < active_count; ++index) {
    T* ptr = (T*)ActivePointers[index];
    if (!file.Write(ptr, sizeof(T))) {
      return false;
    }
  }
  return true;
}

template <class T>
int TFixedIHeapClass<T>::Load(FileClass& file) {
  Clear();

  int active_count = 0;
  if (!file.Read(&active_count, sizeof(active_count))) {
    return false;
  }

  for (int index = 0; index < active_count; ++index) {
    T* ptr = Alloc();
    if (!ptr) {
      return false;
    }
    if (!file.Read(ptr, sizeof(T))) {
      return false;
    }
  }
  return true;
}

template <class T>
void TFixedIHeapClass<T>::Code_Pointers(void) {
  for (int index = 0; index < ActivePointers.Count(); ++index) {
    T* object = (T*)ActivePointers[index];
    if (object) {
      ActivePointers[index] =
          reinterpret_cast<void*>(object->As_Target());
    }
  }
}

template <class T>
void TFixedIHeapClass<T>::Decode_Pointers(void) {
  for (int index = 0; index < ActivePointers.Count(); ++index) {
    auto encoded = reinterpret_cast<std::uintptr_t>(ActivePointers[index]);
    ActivePointers[index] =
        reinterpret_cast<void*>(As_Object(static_cast<TARGET>(
            encoded & std::numeric_limits<TARGET>::max())));
  }
}

// Explicit instantiations for the templated heap types referenced in GLOBALS.
template class TFixedIHeapClass<AnimClass>;
template class TFixedIHeapClass<TemplateClass>;
template class TFixedIHeapClass<TerrainClass>;
template class TFixedIHeapClass<TriggerClass>;
template class TFixedIHeapClass<AircraftClass>;
template class TFixedIHeapClass<BuildingClass>;
template class TFixedIHeapClass<InfantryClass>;
template class TFixedIHeapClass<TeamClass>;
template class TFixedIHeapClass<TeamTypeClass>;
template class TFixedIHeapClass<UnitClass>;
