#pragma once

#include "vector.h"

class FileClass;
class ObjectClass;

class LayerClass : public DynamicVectorClass<ObjectClass*> {
 public:
  void Sort();
  bool Submit(ObjectClass const* object, bool sort = false);
  int Sorted_Add(ObjectClass const* object);

  virtual void Init() { Clear(); }
  virtual void One_Time() {}

  bool Load(FileClass& file);
  bool Save(FileClass& file);
  virtual void Code_Pointers();
  virtual void Decode_Pointers();
};
