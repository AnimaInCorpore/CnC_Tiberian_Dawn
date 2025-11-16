/*
**  Modernized vector helpers for Command & Conquer.
**
**  This header mirrors the semantics of the original VECTOR.H but removes
**  the dependency on Watcom extensions so the templates compile cleanly
**  under modern C++ toolchains.
*/

#ifndef CNC_TIBERIAN_DAWN_SRC_VECTOR_H_
#define CNC_TIBERIAN_DAWN_SRC_VECTOR_H_

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <new>

namespace cnc {

template <class T>
class VectorClass {
 public:
  VectorClass(unsigned size = 0, T const* array = nullptr)
      : Vector(nullptr), VectorMax(0), IsAllocated(false) {
    if (size) {
      Resize(size, array);
    }
  }

  VectorClass(VectorClass const& other)
      : Vector(nullptr), VectorMax(0), IsAllocated(false) {
    *this = other;
  }

  virtual ~VectorClass() { Clear(); }

  T& operator[](unsigned index) { return Vector[index]; }
  T const& operator[](unsigned index) const { return Vector[index]; }

  virtual VectorClass& operator=(VectorClass const& other) {
    if (this == &other) {
      return *this;
    }

    Clear();
    if (other.VectorMax == 0) {
      return *this;
    }

    if (!Resize(other.VectorMax)) {
      return *this;
    }

    for (unsigned index = 0; index < VectorMax; ++index) {
      Vector[index] = other[index];
    }

    return *this;
  }

  virtual int operator==(VectorClass const& other) const {
    if (VectorMax != other.Length()) {
      return false;
    }
    for (unsigned index = 0; index < VectorMax; ++index) {
      if (Vector[index] != other[index]) {
        return false;
      }
    }
    return true;
  }

  virtual int Resize(unsigned newsize, T const* array = nullptr) {
    if (!newsize) {
      Clear();
      return true;
    }

    T* newptr = nullptr;
    bool new_allocated = false;

    if (array) {
      newptr = const_cast<T*>(array);
      for (unsigned i = 0; i < newsize; ++i) {
        new (&newptr[i]) T();
      }
    } else {
      newptr = new (std::nothrow) T[newsize];
      new_allocated = true;
    }

    if (!newptr) {
      return false;
    }

    if (Vector) {
      const unsigned copycount = std::min(VectorMax, newsize);
      for (unsigned index = 0; index < copycount; ++index) {
        newptr[index] = Vector[index];
      }
      if (IsAllocated) {
        delete[] Vector;
      }
    }

    Vector = newptr;
    VectorMax = newsize;
    IsAllocated = new_allocated;
    return true;
  }

  virtual void Clear() {
    if (Vector && IsAllocated) {
      delete[] Vector;
    }
    Vector = nullptr;
    VectorMax = 0;
    IsAllocated = false;
  }

  unsigned Length() const { return VectorMax; }

  virtual int ID(T const* ptr) const {
    if (!Vector || !ptr) {
      return -1;
    }
    if (ptr < Vector || ptr >= Vector + VectorMax) {
      return -1;
    }
    return static_cast<int>(ptr - Vector);
  }

  virtual int ID(T const& object) const {
    for (unsigned index = 0; index < VectorMax; ++index) {
      if (Vector[index] == object) {
        return static_cast<int>(index);
      }
    }
    return -1;
  }

 protected:
  T* Vector;
  unsigned VectorMax;
  bool IsAllocated;
};

template <class T>
class DynamicVectorClass : public VectorClass<T> {
 public:
  DynamicVectorClass(unsigned size = 0, T const* array = nullptr)
      : VectorClass<T>(size, array), ActiveCount(0), GrowthStep(10) {}

  int Count() const { return ActiveCount; }

  virtual int Resize(unsigned newsize, T const* array = nullptr) override {
    if (VectorClass<T>::Resize(newsize, array)) {
      if (this->Length() < static_cast<unsigned>(ActiveCount)) {
        ActiveCount = static_cast<int>(this->Length());
      }
      return true;
    }
    return false;
  }

  void Clear() override {
    ActiveCount = 0;
    VectorClass<T>::Clear();
  }

  int Add(T const& object) {
    if (ActiveCount >= static_cast<int>(this->Length())) {
      if ((this->IsAllocated || !this->VectorMax) && GrowthStep > 0) {
        if (!Resize(this->Length() + GrowthStep)) {
          return false;
        }
      } else {
        return false;
      }
    }

    (*this)[ActiveCount++] = object;
    return true;
  }

  int Add_Head(T const& object) {
    if (ActiveCount >= static_cast<int>(this->Length())) {
      if ((this->IsAllocated || !this->VectorMax) && GrowthStep > 0) {
        if (!Resize(this->Length() + GrowthStep)) {
          return false;
        }
      } else {
        return false;
      }
    }

    if (ActiveCount > 0) {
      std::memmove(&(*this)[1], &(*this)[0],
                   static_cast<std::size_t>(ActiveCount) * sizeof(T));
    }
    (*this)[0] = object;
    ++ActiveCount;
    return true;
  }

  int Delete(T const& object) {
    const int index = ID(object);
    if (index != -1) {
      return Delete(index);
    }
    return false;
  }

  int Delete(int index) {
    if (index < 0 || index >= ActiveCount) {
      return false;
    }

    --ActiveCount;
    for (int i = index; i < ActiveCount; ++i) {
      (*this)[i] = (*this)[i + 1];
    }
    return true;
  }

  void Delete_All() { ActiveCount = 0; }

  int Set_Growth_Step(int step) {
    GrowthStep = step;
    return GrowthStep;
  }

  int Growth_Step() const { return GrowthStep; }

  virtual int ID(T const& object) const override {
    for (int index = 0; index < ActiveCount; ++index) {
      if ((*this)[index] == object) {
        return index;
      }
    }
    return -1;
  }

 protected:
  int ActiveCount;
  int GrowthStep;
};

class BooleanVectorClass {
 public:
  BooleanVectorClass(unsigned size = 0, unsigned char* array = nullptr);
  BooleanVectorClass(BooleanVectorClass const& vector);

  BooleanVectorClass& operator=(BooleanVectorClass const& vector);
  int operator==(BooleanVectorClass const& vector);

  int Length() const { return BitCount; }
  void Reset();
  void Set();
  void Clear();
  int Resize(unsigned size);

  bool const& operator[](int index) const {
    if (LastIndex != index) {
      Fixup(index);
    }
    return Copy;
  }

  bool& operator[](int index) {
    if (LastIndex != index) {
      Fixup(index);
    }
    return Copy;
  }

  bool Is_True(int index) const;
  int First_False() const;
  int First_True() const;

 private:
  void Fixup(int index = -1) const;

  unsigned char* Data();
  unsigned char* Data() const;

  int BitCount = 0;
  mutable bool Copy = false;
  mutable int LastIndex = -1;
  mutable VectorClass<unsigned char> BitArray;
};

namespace detail {
bool GetBooleanBit(unsigned char const* data, int bit);
void SetBooleanBit(unsigned char* data, int bit, bool value);
int FirstTrueBit(unsigned char const* data, int bit_count);
int FirstFalseBit(unsigned char const* data, int bit_count);
}  // namespace detail

}  // namespace cnc

template <class T>
using VectorClass = cnc::VectorClass<T>;

template <class T>
using DynamicVectorClass = cnc::DynamicVectorClass<T>;

using BooleanVectorClass = cnc::BooleanVectorClass;

#endif  // CNC_TIBERIAN_DAWN_SRC_VECTOR_H_
