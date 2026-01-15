#pragma once

#include "gscreen.h"
#include "legacy_compat.h"

// Constants
#ifndef BIGMAP
#define BIGMAP 0
#endif

class MapClass : public GScreenClass {
public:
    /*
    ** Initialization
    */
    virtual void One_Time(void);                                  // Theater-specific inits
    virtual void Init_Clear(void);                                // Clears all to known state
    virtual void Alloc_Cells(void);                               // Allocates buffers
    virtual void Free_Cells(void);                                // Frees buffers
    virtual void Init_Cells(void);                                // Init cell data

    /*--------------------------------------------------------
    ** Main functions that deal with groupings of cells within the map or deals with the cell
    ** as it relates to the map - not what the cell contains.
    */
    ObjectClass* Close_Object(COORDINATE coord) const;
    virtual void Detach(ObjectClass*) {};
    int Cell_Region(CELL cell);
    int Cell_Threat(CELL cell, HousesType house);
    int Cell_Distance(CELL cell1, CELL cell2);
    bool In_Radar(CELL cell) const;
    void Sight_From(CELL cell, int sightrange, bool incremental = false);
    void Place_Down(CELL cell, ObjectClass* object);
    void Pick_Up(CELL cell, ObjectClass* object);
    void Overlap_Down(CELL cell, ObjectClass* object);
    void Overlap_Up(CELL cell, ObjectClass* object);
    bool Read_Binary(char const* root, unsigned long* crc);
    bool Write_Binary(char const* root);
    bool Place_Random_Crate(void);

    long Overpass(void);

    virtual void Logic(void);
    virtual void Set_Map_Dimensions(int x, int y, int w, int h);

    // Return the index (CELL) for a pointer into the internal cell array,
    // or -1 if the pointer is not inside the managed buffer.
    int ID(void const* pointer) const;

    /*
    **	File I/O.
    */
    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

    /*
    ** Debug routine
    */
    int Validate(void);

    /*
    **	This is the dimensions and position of the sub section of the global map.
    **	It is this region that appears on the radar map and constrains normal
    **	movement.
    */
    int MapCellX;
    int MapCellY;
    int MapCellWidth;
    int MapCellHeight;

    /*
    **	This is the total value of all harvestable Tiberium on the map.
    */
    long TotalValue;

protected:
    /*
    **	These are the size dimensions of the underlying array of cell objects.
    **	This is the dimensions of the "map" that the tactical view is
    **	restricted to.
    */
    int XSize;
    int YSize;
    int Size;

    static int const RadiusCount[11];
    static int const RadiusOffset[];

private:
    friend class CellClass;

    /*
    **	Tiberium growth potiential cells are recorded here.
    */
    CELL TiberiumGrowth[50];
    int TiberiumGrowthCount;

    /*
    **	List of cells that are full enough strength that they could spread
    **	Tiberium to adjacent cells.
    */
    CELL TiberiumSpread[50];
    int TiberiumSpreadCount;

    /*
    **	This is the current cell number in the incremental map scan process.
    **  (Renamed from TiberiumScan to match original)
    */
    CELL TiberiumScan;

    /*
    **	If the Tiberium map scan is processing forward, then this flag
    **	will be true. It alternates between forward and backward scanning
    **	in order to avoid the "Tiberium Creep".
    */
    unsigned IsForwardScan : 1;

    enum MapEnum { SCAN_AMOUNT = MAP_CELL_TOTAL };
};

extern MapClass Map;
