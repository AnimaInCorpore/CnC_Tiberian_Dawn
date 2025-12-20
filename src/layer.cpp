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
  if (ActiveCount >= static_cast<int>(Length())) {
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
