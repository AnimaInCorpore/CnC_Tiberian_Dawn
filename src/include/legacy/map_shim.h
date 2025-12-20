/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAP_SHIM_H
#define MAP_SHIM_H

#include "defines.h"
#include "layer.h"
#include <unordered_map>

class ObjectClass;
class ObjectTypeClass;
class TechnoClass;
class BuildingClass;
class UnitClass;
class TriggerClass;

struct MapCellStub {
	MapCellStub() = default;
	HousesType Owner = HOUSE_NONE;
	bool IsVisible = false;
	bool IsMapped = false;
	bool IsFlagged = false;
	bool IsTrigger = false;
	OverlayType Overlay = OVERLAY_NONE;
	unsigned OverlayData = 0;
	struct FlagData {
		struct OccupyData {
			unsigned Center : 1;
			unsigned NW : 1;
			unsigned NE : 1;
			unsigned SW : 1;
			unsigned SE : 1;
			unsigned Vehicle : 1;
			unsigned Monolith : 1;
			unsigned Building : 1;
		} Occupy{};
		unsigned char Composite = 0;
	} Flag;
	HousesType InfType = HOUSE_NONE;
	LandType Land = LAND_CLEAR;
	ObjectClass* Overlapper[4] = {nullptr, nullptr, nullptr, nullptr};

	COORDINATE Closest_Free_Spot(COORDINATE coord, bool = false) const { return coord; }
	bool Cell_Terrain() const { return false; }
	LandType Land_Type() const { return Land; }
	void Incoming(int, bool) {}
	TechnoClass* Cell_Techno() const { return nullptr; }
	BuildingClass* Cell_Building() const { return nullptr; }
	ObjectClass* Cell_Object() const { return nullptr; }
	ObjectClass* Cell_Occupier() const { return nullptr; }
	UnitClass* Cell_Unit() const { return nullptr; }
	bool Flag_Remove() { return false; }
	bool Flag_Place(HousesType) { return false; }
	void Redraw_Objects() {}
	void Recalc_Attributes() {}
	void Wall_Update() {}
	void Adjust_Threat(HousesType, int) {}
	bool Is_Generally_Clear() const { return true; }
	int Reduce_Tiberium(int amount) { return amount; }
	int Reduce_Wall(int damage) { return 0; }
	bool Goodie_Check(ObjectClass*) { return false; }
	TriggerClass* Get_Trigger() const { return nullptr; }
};

struct MapColumnStub {
	void Flag_To_Redraw() { IsToRedraw = true; }
	bool IsToRedraw = false;
};

class MapStubClass
{
	public:
		MapStubClass();
		void One_Time();

		MapCellStub& operator[](CELL cell);
		MapCellStub const& operator[](CELL cell) const;

		void Pick_Up(CELL cell, ObjectClass* object);
		void Place_Down(CELL cell, ObjectClass* object);
		void Refresh_Cells(CELL cell, short const* list);
		void Flag_To_Redraw(bool complete);

		bool Push_Onto_TacMap(COORDINATE source, COORDINATE dest);
		bool Coord_To_Pixel(COORDINATE coord, int& x, int& y) const;
		COORDINATE Pixel_To_Coord(int x, int y) const;
		bool In_Radar(CELL cell) const;
		bool In_View(CELL cell) const;
		void Sight_From(CELL cell, int sightRange, bool incremental = false);
		int Cell_Distance(CELL cell, CELL other) const;
		int Cell_Threat(CELL cell, HousesType house) const;

		CELL Calculated_Cell(SourceType dir, HousesType house);
		ObjectClass* Cell_Object(CELL cell, int x, int y);
		void Detach(ObjectClass* object);
		void Overlap_Down(CELL cell, ObjectClass* object);
		void Overlap_Up(CELL cell, ObjectClass* object);
		void Remove(ObjectClass* object, int layer);
		void Submit(ObjectClass* object, int layer);

		void Add(RTTIType type, int id);
		void Recalc();

		bool Abandon_Production(RTTIType type, int factory);
		bool Add_To_List(ObjectTypeClass const* object);
		bool Factory_Link(int factory, RTTIType type, int id);
		bool Repair_Mode_Control(int value);
		bool Sell_Mode_Control(int value);
		bool Radar_Activate(bool activate);
		COORDINATE Closest_Free_Spot(COORDINATE coord, bool forceclear = false) const;
		void Render(bool complete = false);
		void Flag_Cell(CELL cell);
		void Set_Cursor_Pos(int x, int y);
		void Set_Cursor_Pos(CELL cell);
		void Set_Cursor_Shape(short const* list);
		void Set_Cursor(ObjectTypeClass const* object);
		void Set_Default_Mouse(MouseType mouse, bool wwsmall = false);
		void Input(KeyNumType& input, int x, int y);
		void Set_Tactical_Position(COORDINATE coord);
		bool Activate(int column);
		bool Click_In_Radar(int x, int y, bool exact);
		CELL Click_Cell_Calc(int x, int y);
		void Mouse_Left_Release(CELL cell, int x, int y, ObjectClass* object, ActionType action, bool wwsmall = false);
		void Mouse_Left_Up(bool shadow, ObjectClass* object, ActionType action, bool wwsmall = false);
		void Mouse_Right_Press();
		bool Override_Mouse_Shape(MouseType mouse, bool wwsmall = false);
		bool Player_Names(bool redraw);
		bool Is_Player_Names() const;
		bool Is_Radar_Active() const;
		bool Is_Radar_Existing() const;
		int Get_Tab_Height() const;
		void Help_Text(int text_id, int x = -1, int y = -1, int color = LTGREY, bool quick = false, int cost = 0);
		void Zoom_Mode(CELL cell);
		struct ZoomStub { void Enable() {} void Disable() {}; } Zoom;

		MapColumnStub Column[2];
		LayerClass Layer[LAYER_COUNT];

		HousesType PendingHouse;
		ObjectTypeClass const* PendingObject;
		ObjectClass* PendingObjectPtr;

		int MapCellX;
		int MapCellY;
		int MapCellWidth;
		int MapCellHeight;
		int TacPixelX;
		int TacPixelY;
		int TacLeptonWidth;
		int TacLeptonHeight;

		COORDINATE TacticalCoord;
		COORDINATE DesiredTacticalCoord;
		unsigned SpecialRadarFrame;

		unsigned IsToRedraw : 1;
		bool IsSellMode;
		bool IsTargettingMode;
		bool IsRadarActive;
		bool DoesRadarExist;
		bool IsSidebarActive;
		bool IsZoomed;
		bool PlayerNamesEnabled;

		unsigned char const* FadingShade;
		unsigned char const* UnitShadow;
		unsigned char const* FadingLight;
		unsigned char const* WhiteTranslucentTable;
		unsigned char const* TranslucentTable;
		unsigned char const* RemapTables[HOUSE_COUNT][2];

	private:
		MapCellStub const& Lookup(CELL cell) const;
		std::unordered_map<CELL, MapCellStub> cells_;
};

#endif
