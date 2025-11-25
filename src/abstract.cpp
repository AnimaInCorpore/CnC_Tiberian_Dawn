#include "abstract.h"
#include "legacy/function.h"

int AbstractClass::Distance(TARGET target) const
{
	BuildingClass	*obj = As_Building(target);
	int dist = Distance(As_Coord(target));

	if (obj) {
		dist -= ((obj->Class->Width() + obj->Class->Height()) * (0x100 / 4));
		if (dist < 0) dist = 0;
	}

	return(dist);
}

AbstractTypeClass::AbstractTypeClass(int name, char const * ini)
{
	Name = name;
	strncpy((char *)IniName, ini, sizeof(IniName));
	((char &)IniName[sizeof(IniName)-1]) = '\0';
}

RTTIType AbstractTypeClass::What_Am_I(void) const
{
	return RTTI_NONE;
}

COORDINATE AbstractTypeClass::Coord_Fixup(COORDINATE coord) const
{
	return coord;
}

int AbstractTypeClass::Full_Name(void) const
{
	return Name;
}

unsigned short AbstractTypeClass::Get_Ownable(void) const
{
	return 0;
}
