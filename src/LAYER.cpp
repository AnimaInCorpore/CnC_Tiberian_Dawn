#include "function.h"
#include "layer.h"
#include "object.h"

bool LayerClass::Submit(ObjectClass const* object, bool sort) {
  if (sort) {
    return Sorted_Add(object);
  }
  return Add(const_cast<ObjectClass*>(object));
}

void LayerClass::Sort() {
  for (int index = 0; index < Count() - 1; ++index) {
    if (*(*this)[index + 1] < *(*this)[index]) {
      ObjectClass* temp = (*this)[index + 1];
      (*this)[index + 1] = (*this)[index];
      (*this)[index] = temp;
    }
  }
}

int LayerClass::Sorted_Add(ObjectClass const* object) {
  if (ActiveCount >= Length()) {
    if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
      if (!Resize(Length() + GrowthStep)) {
        return false;
      }
    } else {
      return false;
    }
  }

  int index = 0;
  for (; index < ActiveCount; ++index) {
    if (*(*this)[index] > *object) {
      break;
    }
  }

  for (int i = ActiveCount - 1; i >= index; --i) {
    (*this)[i + 1] = (*this)[i];
  }

  (*this)[index] = const_cast<ObjectClass*>(object);
  ++ActiveCount;
  return true;
}

bool LayerClass::Load(FileClass& file) {
  int count = 0;
  if (file.Read(&count, sizeof(count)) != sizeof(count)) {
    return false;
  }

  Clear();

  for (int i = 0; i < count; ++i) {
    ObjectClass* ptr = nullptr;
    if (file.Read(&ptr, sizeof(ptr)) != sizeof(ptr)) {
      return false;
    }
    Add(ptr);
  }

  return true;
}

bool LayerClass::Save(FileClass& file) {
  int count = Count();
  if (file.Write(&count, sizeof(count)) != sizeof(count)) {
    return false;
  }

  for (int i = 0; i < count; ++i) {
    ObjectClass* ptr = (*this)[i];
    if (file.Write(&ptr, sizeof(ptr)) != sizeof(ptr)) {
      return false;
    }
  }

  return true;
}

void LayerClass::Code_Pointers() {
  for (int i = 0; i < Count(); ++i) {
    ObjectClass* obj = (*this)[i];
    (*this)[i] = reinterpret_cast<ObjectClass*>(obj->As_Target());
  }
}

void LayerClass::Decode_Pointers() {
  for (int i = 0; i < Count(); ++i) {
    TARGET target = reinterpret_cast<TARGET>((*this)[i]);
    (*this)[i] = As_Object(target);
    Check_Ptr((*this)[i], __FILE__, __LINE__);
  }
}
