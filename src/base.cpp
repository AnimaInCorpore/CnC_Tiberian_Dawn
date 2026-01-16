#include "base.h"
#include "map_fwd.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int BaseNodeClass::operator==(BaseNodeClass const& node) { return (Type == node.Type && Coord == node.Coord); }

int BaseNodeClass::operator!=(BaseNodeClass const& node) { return (Type != node.Type || Coord != node.Coord); }

int BaseNodeClass::operator>(BaseNodeClass const&) { return true; }

void BaseClass::Read_INI(char* buffer) {
    char buf[128];
    char uname[10];
    BaseNodeClass node;

    WWGetPrivateProfileString("BASIC", "Player", "GoodGuy", buf, 20, buffer);
    if (HouseTypeClass::From_Name(buf) == HOUSE_GOOD) {
        House = HOUSE_BAD;
    } else {
        House = HOUSE_GOOD;
    }

    int count = WWGetPrivateProfileInt(INI_Name(), "Count", 0, buffer);
    for (int i = 0; i < count; i++) {
        ::snprintf(uname, sizeof(uname), "%03d", i);
        WWGetPrivateProfileString(INI_Name(), uname, NULL, buf, sizeof(buf) - 1, buffer);

        node.Type = BuildingTypeClass::From_Name(::strtok(buf, ","));
        node.Coord = std::atol(::strtok(NULL, ","));

        Nodes.Add(node);
    }
}

void BaseClass::Write_INI(char* buffer) {
    char buf[128];
    char uname[10];

    WWWritePrivateProfileString(INI_Name(), NULL, NULL, buffer);
    WWWritePrivateProfileInt(INI_Name(), "Count", Nodes.Count(), buffer);

    for (int i = 0; i < Nodes.Count(); i++) {
        ::snprintf(uname, sizeof(uname), "%03d", i);
        ::snprintf(buf,
                      sizeof(buf),
                      "%s,%ld",
                      BuildingTypeClass::As_Reference(Nodes[i].Type).IniName,
                      static_cast<long>(Nodes[i].Coord));
        WWWritePrivateProfileString(INI_Name(), uname, buf, buffer);
    }
}

bool BaseClass::Load(FileClass& file) {
    int num_struct;
    int i;
    BaseNodeClass node;

    if (file.Read(&i, sizeof(i)) != sizeof(i)) {
        return (false);
    }
    if (i != (int)sizeof(*this)) {
        return (false);
    }
    if (file.Read(&House, sizeof(House)) != sizeof(House)) {
        return (false);
    }
    if (file.Read(&num_struct, sizeof(num_struct)) != sizeof(num_struct)) {
        return (false);
    }

    for (i = 0; i < num_struct; i++) {
        if (file.Read(&node, sizeof(node)) != sizeof(node)) {
            return (false);
        }
        Nodes.Add(node);
    }
    return (true);
}

bool BaseClass::Save(FileClass& file) {
    int num_struct;
    int i;
    BaseNodeClass node;

    i = sizeof(*this);
    if (file.Write(&i, sizeof(i)) != sizeof(i)) {
        return (false);
    }
    if (file.Write(&House, sizeof(House)) != sizeof(House)) {
        return (false);
    }

    num_struct = Nodes.Count();
    if (file.Write(&num_struct, sizeof(num_struct)) != sizeof(num_struct)) {
        return (false);
    }

    for (i = 0; i < num_struct; i++) {
        node = Nodes[i];
        if (file.Write(&node, sizeof(node)) != sizeof(node)) {
            return (false);
        }
    }
    return (true);
}

bool BaseClass::Is_Built(int index) {
    if (Get_Building(index) != NULL) {
        return (true);
    } else {
        return (false);
    }
}

BuildingClass* BaseClass::Get_Building(int index) {
    BuildingClass* bldg;
    ObjectClass* obj[4];

    CELL cell = Coord_Cell(Nodes[index].Coord);

    obj[0] = Map[cell].Cell_Building();
    obj[1] = Map[cell].Overlapper[0];
    obj[2] = Map[cell].Overlapper[1];
    obj[3] = Map[cell].Overlapper[2];

    bldg = NULL;
    for (int i = 0; i < 4; i++) {
        if (obj[i] && obj[i]->Coord == Nodes[index].Coord && obj[i]->What_Am_I() == RTTI_BUILDING &&
            ((BuildingClass*)obj[i])->Class && ((BuildingClass*)obj[i])->Class->Type == Nodes[index].Type) {
            bldg = (BuildingClass*)obj[i];
            break;
        }
    }

    return (bldg);
}

bool BaseClass::Is_Node(BuildingClass* obj) {
    if (Get_Node(obj) != NULL) {
        return (true);
    } else {
        return (false);
    }
}

BaseNodeClass* BaseClass::Get_Node(BuildingClass* obj) {
    for (int i = 0; i < Nodes.Count(); i++) {
        if (obj && obj->Class && obj->Class->Type == Nodes[i].Type && obj->Coord == Nodes[i].Coord) {
            return (&Nodes[i]);
        }
    }
    return (NULL);
}

BaseNodeClass* BaseClass::Next_Buildable(StructType type) {
    for (int i = 0; i < Nodes.Count(); i++) {
        if (type == STRUCT_NONE) {
            if (!Is_Built(i)) {
                return (&Nodes[i]);
            }
        } else {
            if (Nodes[i].Type == type && !Is_Built(i)) {
                return (&Nodes[i]);
            }
        }
    }
    return (NULL);
}
