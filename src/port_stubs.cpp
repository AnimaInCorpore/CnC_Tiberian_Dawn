#include <SDL.h>

#include "legacy/compat.h"
#include "legacy/ccdde.h"
#include "legacy/function.h"
#include "legacy/event.h"
#include "legacy/externs.h"
#include "legacy/gscreen.h"
#include "legacy/nullmodem_stub.h"
#include "legacy/cdfile.h"
#include "legacy/windows_compat.h"
#include "platform_input.h"
#include "runtime_sdl.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdarg>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <thread>

// Global ready flag and null modem instance expected by legacy code paths.
bool ReadyToQuit = false;
NullModemClass NullModem;
DDEServerClass DDEServer;

BOOL Send_Data_To_DDE_Server(char*, int, int) { return FALSE; }

// Shape buffer placeholders.
long _ShapeBufferSize = 512 * 1024;
char* _ShapeBuffer = nullptr;
bool OverlappedVideoBlits = false;

namespace {

constexpr int kPaletteSize = 256 * 3;

void Ensure_Shape_Buffer() {
  if (_ShapeBuffer || _ShapeBufferSize <= 0) return;
  char* buffer = new (std::nothrow) char[_ShapeBufferSize];
  if (buffer) {
    _ShapeBuffer = buffer;
  } else {
    _ShapeBufferSize = 0;
  }
}

void Ensure_Palette_Buffer(unsigned char*& palette, unsigned char fill) {
  if (!palette) {
    palette = new (std::nothrow) unsigned char[kPaletteSize];
  }
  if (palette) {
    std::fill(palette, palette + kPaletteSize, fill);
  }
}

void Ensure_Palettes() {
  Ensure_Palette_Buffer(BlackPalette, 0);
  Ensure_Palette_Buffer(GamePalette, 0);
  Ensure_Palette_Buffer(OriginalPalette, 0);
  Ensure_Palette_Buffer(WhitePalette, 63);
}

void Configure_Game_Viewports() {
  if (!VisiblePage.Is_Valid()) {
    VisiblePage.Init(ScreenWidth, ScreenHeight, nullptr, 0, GBC_NONE);
  }
  if (!HiddenPage.Is_Valid()) {
    HiddenPage.Init(ScreenWidth, ScreenHeight, nullptr, 0, GBC_NONE);
  }

  const bool is_640x480 = (ScreenHeight == 480);
  const int y_offset = is_640x480 ? 40 : 0;
  const int target_height = is_640x480 ? 400 : ScreenHeight;

  SeenBuff.Configure(&VisiblePage, 0, y_offset, ScreenWidth, target_height);
  HidPage.Configure(&HiddenPage, 0, y_offset, ScreenWidth, target_height);
}

void Reset_Game_State_For_Menu() {
  GameActive = true;
  DoList.Init();
  OutList.Init();
  Frame = 0;
  PlayerWins = false;
  PlayerLoses = false;
  PlayerRestarts = false;

  Map.PendingHouse = HOUSE_NONE;
  Map.PendingObject = nullptr;
  Map.PendingObjectPtr = nullptr;
  Map.SpecialRadarFrame = 0;
  Map.IsSellMode = false;
  Map.IsTargettingMode = false;
  Map.IsRadarActive = false;
  Map.DoesRadarExist = false;
  Map.IsZoomed = false;
  Map.Set_Default_Mouse(MOUSE_NORMAL, false);
}

}  // namespace

int Bound(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

void CCDebugString(char const* string) { std::fputs(string ? string : "", stderr); }

void Mono_Clear_Screen() {}

int Mono_Printf(char const* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = std::vfprintf(stderr, fmt, args);
  std::fputc('\n', stderr);
  va_end(args);
  return result;
}

void* Load_Alloc_Data(FileClass& file) {
  const int size = file.Size();
  if (size <= 0) return nullptr;
  char* buffer = new char[size + 1];
  file.Read(buffer, size);
  buffer[size] = '\0';
  return buffer;
}

void CC_Draw_Shape(void const*, int, int, int, WindowNumberType, unsigned int,
                   void const*, void const*) {}

unsigned short Get_Build_Frame_Width(void const*) { return 0; }
unsigned short Get_Build_Frame_Height(void const*) { return 0; }
int Get_Resolution_Factor() { return 1; }

int Distance_Coord(COORDINATE coord1, COORDINATE coord2) {
  const int dx =
      static_cast<int>(Coord_X(coord2)) - static_cast<int>(Coord_X(coord1));
  const int dy =
      static_cast<int>(Coord_Y(coord2)) - static_cast<int>(Coord_Y(coord1));
  return static_cast<int>(std::sqrt(static_cast<double>(dx * dx + dy * dy)));
}

void Delay(int ticks) {
  if (ticks <= 0) return;
  std::this_thread::sleep_for(std::chrono::milliseconds(ticks));
}

void* Add_Long_To_Pointer(void* ptr, long offset) {
  return static_cast<unsigned char*>(ptr) + offset;
}

void Buffer_To_Page(int, int, int, int, void const*, GraphicBufferClass&) {}

void Shake_Screen(int) {}

bool Queue_Options() { return false; }

void const* Hires_Retrieve(char const* /*name*/) { return nullptr; }

void Validate_Error(char const*) {}

bool Parse_Command_Line(int argc, char** argv) {
  if (!argv) return true;

  std::string cd_disc;
  for (int i = 1; i < argc; ++i) {
    if (!argv[i]) continue;
    std::string arg = argv[i];
    if (arg.empty()) continue;

    std::string lower = arg;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "gdi" || lower == "-gdi" || lower == "--gdi" || lower == "--cd=gdi") {
      cd_disc = "GDI";
      continue;
    }
    if (lower == "nod" || lower == "-nod" || lower == "--nod" || lower == "--cd=nod") {
      cd_disc = "NOD";
      continue;
    }
  }

  if (!cd_disc.empty()) {
    CDFileClass::Set_CD_Subfolder(cd_disc.c_str());
    CCDebugString(cd_disc == "GDI" ? "Using GDI disc assets.\n" : "Using NOD disc assets.\n");
  }

  return true;
}

void Read_Setup_Options(RawFileClass*) {}

void Reset_Theater_Shapes() {}

int Version_Number() { return 1; }

int WWGetPrivateProfileInt(char const*, char const*, int def, char*) {
  return def;
}

char* WWGetPrivateProfileString(char const*, char const*, char const* def,
                              char* dst, int dstlen, char*) {
  if (!dst || dstlen <= 0) return nullptr;
  std::strncpy(dst, def ? def : "", dstlen);
  dst[dstlen - 1] = '\0';
  return dst;
}

bool WWWritePrivateProfileString(char const*, char const*, char const* value,
                                char*) {
  return value ? true : false;
}

TheaterType Theater_From_Name(char const* name) {
  if (!name) return THEATER_NONE;
  for (int index = 0; index < THEATER_COUNT; ++index) {
    if (strcasecmp(name, Theaters[index].Name) == 0) {
      return static_cast<TheaterType>(index);
    }
  }
  return THEATER_NONE;
}

bool Read_Private_Config_Struct(char*, NewConfigType* config) {
  if (config) {
    std::memset(config, 0, sizeof(*config));
  }
  return true;
}

void Check_Use_Compressed_Shapes() {}

unsigned long Disk_Space_Available() { return 1024 * 1024 * 1024; }

int Ram_Free() { return 16 * 1024 * 1024; }

void Memory_Error_Handler(void) {}

COORDINATE As_Coord(TARGET) { return 0; }
ObjectClass* As_Object(TARGET) { return nullptr; }
BuildingClass* As_Building(TARGET) { return nullptr; }

void Draw_Box(int x, int y, int w, int h, BoxStyleEnum style, bool filled) {
  GraphicViewPortClass* page = LogicPage ? LogicPage : &HidPage;
  if (!page) return;

  struct BoxColors {
    int fill;
    int highlight;
    int shadow;
  };

  BoxColors colors{DKGREY, WHITE, BLACK};
  switch (style) {
    case BOXSTYLE_DOWN:
      colors = {DKGREY, BLACK, LTGREY};
      break;
    case BOXSTYLE_DIS_DOWN:
    case BOXSTYLE_DIS_RAISED:
      colors = {LTGREY, DKGREY, BLACK};
      break;
    case BOXSTYLE_GREEN_DOWN:
    case BOXSTYLE_GREEN_BOX:
    case BOXSTYLE_GREEN_BORDER:
      colors = {CC_GREEN, BLACK, DKGREY};
      break;
    case BOXSTYLE_GREEN_RAISED:
      colors = {CC_GREEN, WHITE, DKGREY};
      break;
    default:
      break;
  }

  const int right = x + w - 1;
  const int bottom = y + h - 1;

  if (filled) {
    page->Fill_Rect(x, y, right, bottom, colors.fill);
  }

  // Top/left edges.
  page->Draw_Line(x, y, right, y, colors.highlight);
  page->Draw_Line(x, y, x, bottom, colors.highlight);

  // Bottom/right edges.
  page->Draw_Line(x, bottom, right, bottom, colors.shadow);
  page->Draw_Line(right, y, right, bottom, colors.shadow);
}

bool Confine_Rect(int* x, int* y, int width, int height, int max_width, int max_height) {
  if (!x || !y) return false;
  bool adjusted = false;
  const int max_x = MAX(0, max_width - width);
  const int max_y = MAX(0, max_height - height);
  if (*x < 0) {
    *x = 0;
    adjusted = true;
  } else if (*x > max_x) {
    *x = max_x;
    adjusted = true;
  }
  if (*y < 0) {
    *y = 0;
    adjusted = true;
  } else if (*y > max_y) {
    *y = max_y;
    adjusted = true;
  }
  return adjusted;
}

void Game_Startup(void*, int, int, int, bool) {}

// -----------------------------------------------------------------------------
// Missing gameplay/loop hooks
// -----------------------------------------------------------------------------
EventClass::EventClass(EventType type) : Type(type), Frame(0), ID(0), IsExecuted(0), MPlayerID(0) {
  std::memset(&Data, 0, sizeof(Data));
}

bool Init_Game(int, char**) {
  static bool initialized = false;
  CCDebugString("Init_Game: initializing runtime scaffolding.\n");

  ReadyToQuit = false;
  Ensure_Shape_Buffer();
  Ensure_Palettes();
  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();

  if (!initialized) {
    // Prime the palette buffers so menu fades have valid targets.
    if (Palette && GamePalette) {
      std::memcpy(GamePalette, Palette, kPaletteSize);
    }
    initialized = true;
  }
  return true;
}

bool Select_Game(bool fade) {
  if (ReadyToQuit) {
    return false;
  }

  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();

  GameToPlay = GAME_NORMAL;
  PlaybackGame = 0;
  RecordGame = 0;

  if (fade && GamePalette) {
    Fade_Palette_To(GamePalette, FADE_PALETTE_MEDIUM, nullptr);
  }

  const int selection = Main_Menu(0);
  if (selection < 0) {
    ReadyToQuit = true;
    return false;
  }

  switch (selection) {
    case 0:
      GameToPlay = GAME_NORMAL;
      break;
    case 1:
      GameToPlay = GAME_NORMAL;  // Load/save not wired yet.
      break;
    case 2:
      GameToPlay = GAME_IPX;
      break;
    case 3:
      GameToPlay = GAME_INTERNET;
      break;
    case 4:
      GameToPlay = GAME_NORMAL;
      break;
    case 5:
    default:
      ReadyToQuit = true;
      return false;
  }

  return true;
}

#include "legacy/map.h"

bool Main_Loop() {
  Configure_Game_Viewports();

  while (!ReadyToQuit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_WINDOWEVENT &&
           event.window.event == SDL_WINDOWEVENT_CLOSE)) {
        ReadyToQuit = true;
        continue;
      }

#if defined(TD_PORT_USE_SDL2)
      Platform_Handle_Sdl_Event(event);
#endif
    }

    if (ReadyToQuit) {
      break;
    }

    GadgetClass* gadgets = GScreenClass::Buttons;
    if (gadgets) {
      gadgets->Input();
    }

    if (SpecialDialog == SDLG_NONE) {
      Map.Render();
    }

    GScreenClass::Blit_Display();
    ++Frame;
    SDL_Delay(16);
  }
  return true;
}

bool Map_Edit_Loop() { return true; }

void Modem_Signoff() {
  if (PlaybackGame) return;
  EventClass event(EventClass::EXIT);
  NullModem.Send_Message(&event, sizeof(EventClass), 0);
  NullModem.Send_Message(&event, sizeof(EventClass), 0);
}

void Shutdown_Network() {
  delete[] MetaPacket;
  MetaPacket = nullptr;
  MPlayerGameName[0] = '\0';
}

void Special_Dialog() { CCDebugString("Special dialog placeholder.\n"); }

int Surrender_Dialog() {
  CCDebugString("Surrender requested; defaulting to acceptance.\n");
  return 1;
}

void Free_Scenario_Descriptions() {}
