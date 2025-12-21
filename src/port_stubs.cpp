#include <SDL.h>

#include "legacy/compat.h"
#include "legacy/ccdde.h"
#include "legacy/function.h"
#include "legacy/event.h"
#include "legacy/connect.h"
#include "legacy/externs.h"
#include "legacy/gscreen.h"
#include "legacy/intro.h"
#include "legacy/nullmgr.h"
#include "legacy/cdfile.h"
#include "legacy/windows_compat.h"
#include "legacy/logic.h"
#include "legacy/options.h"
#include "legacy/wwlib32.h"
#include "legacy/jshell.h"
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
#include <vector>

// Global ready flag and null modem instance expected by legacy code paths.
bool ReadyToQuit = false;
NullModemClass NullModem(
    16,
    64,
    (200 / sizeof(EventClass)) * sizeof(EventClass) + sizeof(CommHeaderType),
    0x1234);
DDEServerClass DDEServer;

BOOL Send_Data_To_DDE_Server(char*, int, int) { return FALSE; }

// Shape buffer backing store; allocated lazily to match the original runtime.
long _ShapeBufferSize = 512 * 1024;
char* _ShapeBuffer = nullptr;
bool OverlappedVideoBlits = false;
bool StreamLowImpact = false;

namespace {

constexpr int kPaletteSize = 256 * 3;
constexpr unsigned long kMenuTimeoutMs = 60 * 1000;
constexpr int kDefaultFrameMs = 66;

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

void Configure_New_Game_From_Menu() {
  CarryOverMoney = 0;
  Scenario = 1;
  BuildLevel = 1;
  ScenPlayer = SCEN_PLAYER_GDI;
  ScenDir = SCEN_DIR_EAST;
  Whom = HOUSE_GOOD;

#ifndef DEMO
  Choose_Side();
#endif

  if (Special.IsJurassic && AreThingiesEnabled) {
    ScenPlayer = SCEN_PLAYER_JP;
    ScenDir = SCEN_DIR_EAST;
  }
}

struct WsaAnimationHandle {
  std::vector<unsigned char> owned;
  const void* data = nullptr;
};

GraphicViewPortClass& Active_Draw_Page() {
  if (LogicPage) return *LogicPage;
  return HidPage;
}

unsigned char Apply_Translucent_Table(unsigned char src_color, unsigned char dst_color,
                                      void const* table) {
  if (!table) return src_color;
  const auto* bytes = static_cast<const unsigned char*>(table);
  const unsigned char entry = bytes[src_color];
  if (entry == 0xFF) return src_color;
  return bytes[256 + static_cast<int>(entry) * 256 + dst_color];
}

int Target_Frame_Milliseconds() {
  if (Options.GameSpeed > 0 && Options.GameSpeed < 500) {
    return static_cast<int>(Options.GameSpeed);
  }
  return kDefaultFrameMs;
}

void Pump_Sdl_Events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT ||
        (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
      ReadyToQuit = true;
      continue;
    }

    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        GameInFocus = true;
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        GameInFocus = false;
      }
    }

#if defined(TD_PORT_USE_SDL2)
    Platform_Handle_Sdl_Event(event);
#endif
  }
}
}  // namespace

int Bound(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

void CCDebugString(char const* string) { std::fputs(string ? string : "", stderr); }


// Mono helpers are implemented in src/monoc.cpp for the port build.

void* Load_Alloc_Data(FileClass& file) {
  const bool was_open = file.Is_Open();
  if (!was_open && !file.Open(READ)) {
    return nullptr;
  }

  const int size = file.Size();
  if (size <= 0) {
    if (!was_open) file.Close();
    return nullptr;
  }

  char* buffer = new char[size + 1];
  const long read = file.Read(buffer, size);
  if (!was_open) file.Close();

  if (read != size) {
    delete[] buffer;
    return nullptr;
  }

  buffer[size] = '\0';
  return buffer;
}

void* Open_Animation(char const* name, void* buffer, long length, WSAOpenType flags,
                     void* palette) {
  auto* handle = new (std::nothrow) WsaAnimationHandle();
  if (!handle) return nullptr;

  const bool from_mem = (flags & WSA_OPEN_FROM_MEM) != 0;
  if (from_mem && buffer && length > 0) {
    handle->data = buffer;
  } else {
    void const* mix_ptr = MixFileClass::Retrieve(name);
    if (mix_ptr) {
      handle->data = mix_ptr;
    } else {
      CCFileClass file(name);
      if (!file.Open(READ)) {
        delete handle;
        return nullptr;
      }

      const long size = file.Size();
      if (size <= 0) {
        file.Close();
        delete handle;
        return nullptr;
      }

      handle->owned.resize(static_cast<std::size_t>(size));
      const long read = file.Read(handle->owned.data(), size);
      file.Close();
      if (read != size) {
        delete handle;
        return nullptr;
      }
      handle->data = handle->owned.data();
    }
  }

  if (palette && handle->data) {
    Get_Build_Frame_Palette(handle->data, palette);
  }

  return handle;
}

void Animate_Frame(void const* anim, GraphicBufferClass& dest, int frame) {
  if (!anim) return;
  const auto* handle = static_cast<const WsaAnimationHandle*>(anim);
  if (!handle->data) return;

  const int width = static_cast<int>(Get_Build_Frame_Width(handle->data));
  const int height = static_cast<int>(Get_Build_Frame_Height(handle->data));
  if (width <= 0 || height <= 0) return;

  static std::vector<unsigned char> temp;
  temp.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  Build_Frame(handle->data, static_cast<unsigned short>(frame), temp.data());

  const int dst_width = std::min(width, dest.Get_Width());
  const int dst_height = std::min(height, dest.Get_Height());
  unsigned char* dst = dest.Get_Buffer();
  const int pitch = dest.Get_Width();

  for (int y = 0; y < dst_height; ++y) {
    std::memcpy(dst + y * pitch, temp.data() + y * width, static_cast<std::size_t>(dst_width));
  }
}

int Get_Animation_Frame_Count(void const* anim) {
  if (!anim) return 0;
  const auto* handle = static_cast<const WsaAnimationHandle*>(anim);
  return handle->data ? static_cast<int>(Get_Build_Frame_Count(handle->data)) : 0;
}

void Close_Animation(void* anim) {
  delete static_cast<WsaAnimationHandle*>(anim);
}

int Extract_Shape_Count(void const* shape) {
  if (!shape) return 0;
  return static_cast<int>(Get_Build_Frame_Count(Get_Shape_Header_Data(const_cast<void*>(shape))));
}

void Wait_Blit(void) {}

void CC_Draw_Shape(void const* shapefile, int shapenum, int x, int y, WindowNumberType window,
                   unsigned int flags, void const* fadingdata, void const* ghostdata) {
  if (!shapefile) return;

  shapefile = Get_Shape_Header_Data(const_cast<void*>(shapefile));

  const int width = static_cast<int>(Get_Build_Frame_Width(shapefile));
  const int height = static_cast<int>(Get_Build_Frame_Height(shapefile));
  if (width <= 0 || height <= 0) return;

  if ((flags & SHAPE_CENTER) != 0) {
    x -= width / 2;
    y -= height / 2;
  }
  if ((flags & SHAPE_WIN_REL) != 0) {
    x += WindowList[window][WINDOWX] << 3;
    y += WindowList[window][WINDOWY];
  }

  static std::vector<unsigned char> temp;
  temp.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  Build_Frame(shapefile, static_cast<unsigned short>(shapenum), temp.data());

  GraphicViewPortClass& dest_view = Active_Draw_Page();
  unsigned char* dst = static_cast<unsigned char*>(dest_view.Get_Offset());
  const int dst_pitch = dest_view.Get_Pitch();

  const int clip_x0 = dest_view.Get_XPos();
  const int clip_y0 = dest_view.Get_YPos();
  const int clip_x1 = clip_x0 + dest_view.Get_Width();
  const int clip_y1 = clip_y0 + dest_view.Get_Height();

  for (int sy = 0; sy < height; ++sy) {
    const int dy = y + sy;
    if (dy < clip_y0 || dy >= clip_y1) continue;

    for (int sx = 0; sx < width; ++sx) {
      const int dx = x + sx;
      if (dx < clip_x0 || dx >= clip_x1) continue;

      unsigned char src_color = temp[static_cast<std::size_t>(sy) * width + sx];
      if (src_color == 0) continue;

      if (fadingdata && ((flags & SHAPE_FADING) != 0)) {
        src_color = static_cast<const unsigned char*>(fadingdata)[src_color];
      }

      unsigned char* dst_pixel = dst + (dy - clip_y0) * dst_pitch + (dx - clip_x0);
      const unsigned char dst_color = *dst_pixel;

      if (ghostdata && (((flags & SHAPE_GHOST) != 0) || ((flags & SHAPE_TRANS) != 0))) {
        src_color = Apply_Translucent_Table(src_color, dst_color, ghostdata);
      }

      *dst_pixel = src_color;
    }
  }
}

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

// Dialog_Box is implemented in src/dialog.cpp


void Call_Back() {}

void* Add_Long_To_Pointer(void* ptr, long offset) {
  return static_cast<unsigned char*>(ptr) + offset;
}

void Shake_Screen(int) {}


void const* Hires_Retrieve(char const* name) { return MixFileClass::Retrieve(name); }

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

bool WWWritePrivateProfileInt(char const* /*section*/, char const* /*entry*/, int value, char* /*profile*/) {
  // Minimal behavior: writing succeeds when value is valid.
  return value >= 0;
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
// Note: EventClass constructors are implemented in event.cpp; avoid providing
// duplicate definitions here to prevent linker conflicts.
// -----------------------------------------------------------------------------

bool Init_Game(int, char**) {
  static bool initialized = false;
  CCDebugString("Init_Game: initializing runtime scaffolding.\n");

  ReadyToQuit = false;
  AllDone = 0;
  GameInFocus = true;
  SpecialDialog = SDLG_NONE;

  Ensure_Shape_Buffer();
  Ensure_Palettes();
  if (!Palette) {
    Palette = GamePalette;
  }
  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();
  Options.GameSpeed = Target_Frame_Milliseconds();
  TickCount.Reset(0);
  ProcessTimer.Reset(0);
  FrameTimer.Clear();

  Logic.One_Time();
  Options.One_Time();
  Map.One_Time();

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

  DDEServer.Enable();
  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();

  GameToPlay = GAME_NORMAL;
  PlaybackGame = 0;
  RecordGame = 0;
  Options.GameSpeed = Target_Frame_Milliseconds();

  if (fade && GamePalette) {
    Fade_Palette_To(GamePalette, FADE_PALETTE_MEDIUM, nullptr);
  }

  Keyboard::Clear();
  TickCount.Reset(0);

  const int selection = Main_Menu(kMenuTimeoutMs);
  if (selection < 0) {
    ReadyToQuit = true;
    return false;
  }

  switch (selection) {
    case 0:  // New missions (expansion)
      Configure_New_Game_From_Menu();
      GameToPlay = GAME_NORMAL;
      break;
    case 1:  // Start new game
      Configure_New_Game_From_Menu();
      GameToPlay = GAME_NORMAL;
      break;
    case 2:  // Bonus missions
      GameToPlay = GAME_NORMAL;
      break;
    case 3:  // Internet
      GameToPlay = GAME_INTERNET;
      break;
    case 4:  // Load mission (not wired yet)
      GameToPlay = GAME_NORMAL;
      break;
    case 5:  // Multiplayer
      GameToPlay = GAME_IPX;
      break;
    case 6:  // Intro
      // Play the intro/choose-side sequence and then re-open the menu.
      Choose_Side();
      // Re-enter Select_Game so the user returns to the main menu after the intro.
      return Select_Game(false);
    case 7:  // Exit
    default:
      ReadyToQuit = true;
      return false;
  }

  return true;
}

#include "legacy/map.h"

bool Main_Loop() {
  Configure_Game_Viewports();

  const int frame_ms = Target_Frame_Milliseconds();

  while (!ReadyToQuit) {
    const auto frame_start = std::chrono::steady_clock::now();
    Pump_Sdl_Events();

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

    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - frame_start)
                                .count();
    if (elapsed_ms < frame_ms) {
      SDL_Delay(static_cast<Uint32>(frame_ms - elapsed_ms));
    }
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

int Surrender_Dialog() {
  CCDebugString("Surrender requested; defaulting to acceptance.\n");
  return 1;
}

void Free_Scenario_Descriptions() {}
