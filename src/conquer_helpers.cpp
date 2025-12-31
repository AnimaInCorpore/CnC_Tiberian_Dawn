#include "legacy/function.h"

#include <cstdio>
#include <cstring>

void Unselect_All(void) {
  while (CurrentObject.Count()) {
    CurrentObject[0]->Unselect();
  }
}

char const* Fading_Table_Name(char const* base, TheaterType theater) {
  static char buffer[256];
  std::snprintf(buffer, sizeof(buffer), "%c%s.MRF", Theaters[theater].Root[0], base);
  return buffer;
}

void const* Get_Radar_Icon(void const* shapefile, int shapenum, int frames, int zoomfactor) {
  static int offx[] = {0, 0, -1, 1, 0, -1, 1, -1, 1};
  static int offy[] = {0, 0, -1, 1, 0, -1, 1, -1, 1};

  if (!shapefile) return nullptr;

  const int pixel_width = Get_Build_Frame_Width(shapefile);
  const int pixel_height = Get_Build_Frame_Height(shapefile);
  const int icon_width = (pixel_width + 12) / 24;
  const int icon_height = (pixel_height + 12) / 24;

  if (frames == -1) frames = Get_Build_Frame_Count(shapefile);

  char* buffer = new char[(icon_width * icon_height * 9 * frames) + 2];
  if (!buffer) return nullptr;

  char* retval = buffer;
  *buffer++ = static_cast<char>(icon_width);
  *buffer++ = static_cast<char>(icon_height);
  const int val = 24 / zoomfactor;

  for (int framelp = 0; framelp < frames; framelp++) {
    void* ptr = Build_Frame(shapefile, shapenum + framelp, SysMemPage.Get_Buffer());
    if (ptr) {
      ptr = Get_Shape_Header_Data(ptr);
      for (int icony = 0; icony < icon_height; icony++) {
        for (int iconx = 0; iconx < icon_width; iconx++) {
          for (int y = 0; y < zoomfactor; y++) {
            for (int x = 0; x < zoomfactor; x++) {
              int getx = (iconx * 24) + (x * val) + (zoomfactor / 2);
              int gety = (icony * 24) + (y * val) + (zoomfactor / 2);
              if ((getx < pixel_width) && (gety < pixel_height)) {
                char pixel = 0;
                for (int lp = 0; lp < 9; lp++) {
                  pixel = *reinterpret_cast<char*>(
                      Add_Long_To_Pointer(ptr, ((gety - offy[lp]) * pixel_width) + getx - offx[lp]));
                  if (pixel == LTGREEN) pixel = 0;
                  if (pixel) {
                    break;
                  }
                }
                *buffer++ = pixel;
              } else {
                *buffer++ = 0;
              }
            }
          }
        }
      }
    } else {
      buffer += icon_width * icon_height * 9;
    }
  }
  return retval;
}

TechnoTypeClass const* Fetch_Techno_Type(RTTIType type, int id) {
  switch (type) {
    case RTTI_UNITTYPE:
    case RTTI_UNIT:
      return &UnitTypeClass::As_Reference(static_cast<UnitType>(id));
    case RTTI_BUILDINGTYPE:
    case RTTI_BUILDING:
      return &BuildingTypeClass::As_Reference(static_cast<StructType>(id));
    case RTTI_INFANTRYTYPE:
    case RTTI_INFANTRY:
      return &InfantryTypeClass::As_Reference(static_cast<InfantryType>(id));
    case RTTI_AIRCRAFTTYPE:
    case RTTI_AIRCRAFT:
      return &AircraftTypeClass::As_Reference(static_cast<AircraftType>(id));
    default:
      break;
  }
  return nullptr;
}

void Keyboard_Process(KeyNumType& input) {
  if (input == KN_NONE) {
    return;
  }

  if (input == KN_SPACE || input == KN_ESC) {
    Map.Help_Text(TXT_NONE);
    Queue_Options();
    input = KN_NONE;
  }
}
