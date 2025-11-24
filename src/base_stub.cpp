#include "legacy/function.h"
#include "legacy/base.h"

int BaseNodeClass::operator==(BaseNodeClass const& node) {
  return Type == node.Type && Coord == node.Coord;
}

int BaseNodeClass::operator!=(BaseNodeClass const& node) { return !(*this == node); }

int BaseNodeClass::operator>(BaseNodeClass const& node) { return Type > node.Type; }

void BaseClass::Read_INI(char*) {}

void BaseClass::Write_INI(char*) {}

bool BaseClass::Load(FileClass&) { return false; }

bool BaseClass::Save(FileClass&) { return false; }

bool BaseClass::Is_Built(int) { return false; }

BuildingClass* BaseClass::Get_Building(int) { return nullptr; }

bool BaseClass::Is_Node(BuildingClass*) { return false; }

BaseNodeClass* BaseClass::Get_Node(BuildingClass*) { return nullptr; }

BaseNodeClass* BaseClass::Next_Buildable(StructType) { return nullptr; }
