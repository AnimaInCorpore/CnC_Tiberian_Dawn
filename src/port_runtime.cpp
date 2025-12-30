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
#include "legacy/ccfile.h"
#include "legacy/windows_compat.h"
#include "legacy/logic.h"
#include "legacy/options.h"
#include "legacy/loaddlg.h"
#include "legacy/wwlib32.h"
#include "legacy/jshell.h"
#include "legacy/msglist.h"
#include "legacy/defines.h"
#include "legacy/wwalloc.h"
#include "platform_input.h"
#include "runtime_sdl.h"
#include "port_debug.h"
#include "port_paths.h"
#include "port_setup.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdarg>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
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

// Shape buffer backing store; allocated lazily to match the original runtime.
long _ShapeBufferSize = 512 * 1024;
char* _ShapeBuffer = nullptr;
bool OverlappedVideoBlits = false;
bool StreamLowImpact = false;

namespace {

constexpr int kPaletteSize = 256 * 3;
constexpr unsigned long kMenuTimeoutMs = 60 * 1000;

void Ensure_Object_Heaps() {
  static bool initialized = false;
  if (initialized) return;
  initialized = true;

  Units.Set_Heap(UNIT_MAX);
  Factories.Set_Heap(FACTORY_MAX);
  Terrains.Set_Heap(TERRAIN_MAX);
  Templates.Set_Heap(TEMPLATE_MAX);
  Smudges.Set_Heap(SMUDGE_MAX);
  Overlays.Set_Heap(OVERLAY_MAX);
  Infantry.Set_Heap(INFANTRY_MAX);
  Bullets.Set_Heap(BULLET_MAX);
  Buildings.Set_Heap(BUILDING_MAX);
  Anims.Set_Heap(ANIM_MAX);
  Aircraft.Set_Heap(AIRCRAFT_MAX);
  Triggers.Set_Heap(TRIGGER_MAX);
  TeamTypes.Set_Heap(TEAMTYPE_MAX);
  Teams.Set_Heap(TEAM_MAX);
  Houses.Set_Heap(HOUSE_MAX);
}

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
    if (palette) {
      std::fill(palette, palette + kPaletteSize, fill);
    }
  }
}

void Ensure_Palettes() {
  Ensure_Palette_Buffer(BlackPalette, 0);
  Ensure_Palette_Buffer(GamePalette, 0);
  Ensure_Palette_Buffer(OriginalPalette, 0);
  Ensure_Palette_Buffer(WhitePalette, 63);

  static bool seeded = false;
  if (seeded) return;
  seeded = true;

  // Seed a canonical base palette early so UI color indices (including the
  // hard-coded CC_GREEN_* range) have meaningful RGB values before the title
  // screen applies its palette.
  unsigned char pal[kPaletteSize]{};
  CCFileClass file("TEMPERAT.PAL");
  if (file.Is_Available() && file.Read(pal, kPaletteSize) == kPaletteSize) {
    std::memcpy(GamePalette, pal, kPaletteSize);
    std::memcpy(OriginalPalette, pal, kPaletteSize);
    if (Palette) {
      std::memcpy(Palette, pal, kPaletteSize);
    }
  }
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
  const unsigned int speed = Options.GameSpeed;
  const unsigned int ticks_per_frame = (speed == 0u) ? 1u : speed;
  const unsigned int clamped =
      std::clamp(ticks_per_frame, 1u, static_cast<unsigned int>(TIMER_SECOND));
  const unsigned int ms = (clamped * 1000u + (TIMER_SECOND / 2u)) / static_cast<unsigned int>(TIMER_SECOND);
  return static_cast<int>(std::max(1u, ms));
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

extern void Keyboard_Process(KeyNumType& input);

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

void Wait_Blit(void) {
  Pump_Sdl_Events();
  AllSurfaces.Set_Surface_Focus(GameInFocus);
  SDL_Delay(0);
}

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
  const long long ms = (static_cast<long long>(ticks) * 1000 + (TIMER_SECOND / 2)) / TIMER_SECOND;
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Dialog_Box is implemented in src/dialog.cpp


void Call_Back() {
  Pump_Sdl_Events();
  AllSurfaces.Set_Surface_Focus(GameInFocus);
  GScreenClass::Blit_Display();
  SDL_Delay(1);
}

void* Add_Long_To_Pointer(void* ptr, long offset) {
  return static_cast<unsigned char*>(ptr) + offset;
}

void Shake_Screen(int) {}


void const* Hires_Retrieve(char const* name) { return MixFileClass::Retrieve(name); }

void Validate_Error(char const* name) {
#ifdef CHEAT_KEYS
  Prog_End();
  std::printf("%s object error!\n", name ? name : "(null)");
  std::exit(0);
#else
  (void)name;
#endif
}

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

    if (lower == "-d" || lower == "--debug" || lower == "--verbose" || lower == "-v") {
      Debug_Flag = true;
      SDL_setenv("TD_VERBOSE", "1", 1);
      TD_Debugf("Debug mode enabled via argv: %s", arg.c_str());
      continue;
    }

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
    TD_Debugf("CD subfolder set to: %s", cd_disc.c_str());
  }

  return true;
}

void Read_Setup_Options(RawFileClass*);

void Reset_Theater_Shapes(void);

int Version_Number() { return 1; }

TheaterType Theater_From_Name(char const* name) {
  if (!name) return THEATER_NONE;
  for (int index = 0; index < THEATER_COUNT; ++index) {
    if (strcasecmp(name, Theaters[index].Name) == 0) {
      return static_cast<TheaterType>(index);
    }
  }
  return THEATER_NONE;
}


void Check_Use_Compressed_Shapes(void);

unsigned long Disk_Space_Available() {
  try {
    const auto space = std::filesystem::space(std::filesystem::current_path());
    return static_cast<unsigned long>(std::min<std::uintmax_t>(
        space.available, static_cast<std::uintmax_t>(std::numeric_limits<unsigned long>::max())));
  } catch (...) {
    return 0;
  }
}

int Ram_Free() {
  const long bytes = ::Ram_Free(MEM_NORMAL);
  if (bytes <= 0) return 0;
  return static_cast<int>(std::min<long>(bytes, std::numeric_limits<int>::max()));
}

void Memory_Error_Handler(void) {
  VisiblePage.Clear();
  Set_Palette(GamePalette);

  while (Get_Mouse_State()) {
    Show_Mouse();
  }

  CCMessageBox().Process("Error - out of memory.", "Abort", nullptr, nullptr, false);
  Prog_End();
  ReadyToQuit = true;
  std::exit(0);
}

bool Force_CD_Available(int cd) {
  // The SDL port typically runs with all assets available locally (either from MIX
  // archives on disk or via configured CD subfolders). Treat "-2" as "no CD needed"
  // (matches the legacy convention) and accept other values for now.
  if (cd == -2) return true;
  return true;
}

void Map_Selection(void) {
  // The legacy build transitions to the campaign globe or multiplayer map selection.
  // For the current port milestone, end the scenario so we return to the main menu.
  GameActive = false;
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

// -----------------------------------------------------------------------------
// Missing gameplay/loop hooks
// Note: EventClass constructors are implemented in event.cpp; avoid providing
// duplicate definitions here to prevent linker conflicts.
// -----------------------------------------------------------------------------

namespace {
bool Required_Data_Available() {
  const bool has_mix = CCFileClass("GENERAL.MIX").Is_Available() || CCFileClass("CONQUER.MIX").Is_Available();
  const std::string ini_path = TD_Resolve_Profile_Read("CONQUER.INI");
  bool has_ini = RawFileClass(ini_path.c_str()).Is_Available();
  if (!has_ini) {
    Ensure_Default_Conquer_Ini();
    const std::string ini_path_retry = TD_Resolve_Profile_Read("CONQUER.INI");
    has_ini = RawFileClass(ini_path_retry.c_str()).Is_Available();
  }
  if (has_mix && has_ini) return true;

  std::fprintf(stderr, "Missing required game assets.\n");
  if (!has_mix) {
    std::fprintf(stderr,
                 "Expected a repo-local `CD/` asset mirror (e.g. `CD/TIBERIAN_DAWN/CD2/GENERAL.MIX`).\n");
  }
  if (!has_ini) {
    std::fprintf(stderr, "Expected `CONQUER.INI` in the working directory or per-user config dir.\n");
  }
  std::fprintf(stderr, "Run from the repository root or ensure the `CD/` folder exists relative to the working directory.\n");
  return false;
}
}  // namespace

bool Init_Game(int, char**) {
  static bool initialized = false;
  CCDebugString("Init_Game: initializing runtime scaffolding.\n");
  TD_Debugf("Init_Game: begin");

  if (!Required_Data_Available()) {
    return false;
  }

  ReadyToQuit = false;
  AllDone = 0;
  GameInFocus = true;
  SpecialDialog = SDLG_NONE;

  Ensure_Object_Heaps();
  Ensure_Shape_Buffer();
  Ensure_Palettes();
  if (!Palette) {
    Palette = GamePalette;
  }
  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();
  TickCount.Reset(0);
  ProcessTimer.Reset(0);
  FrameTimer.Clear();

  Logic.One_Time();
  Options.One_Time();
  Map.One_Time();
  TD_Debugf("Init_Game: One_Time complete (Logic/Options/Map)");

  if (!initialized) {
    // Prime the palette buffers so menu fades have valid targets.
    if (Palette && GamePalette) {
      std::memcpy(GamePalette, Palette, kPaletteSize);
    }
    initialized = true;
  }
  TD_Debugf("Init_Game: done");
  return true;
}

bool Select_Game(bool fade) {
  if (ReadyToQuit) {
    return false;
  }

  TD_Debugf("Select_Game: enter fade=%s", fade ? "true" : "false");
  DDEServer.Enable();
  Configure_Game_Viewports();
  Reset_Game_State_For_Menu();

  bool gameloaded = false;

  GameToPlay = GAME_NORMAL;
  PlaybackGame = 0;
  RecordGame = 0;

  if (fade && GamePalette) {
    Fade_Palette_To(GamePalette, FADE_PALETTE_MEDIUM, nullptr);
  }

  Keyboard::Clear();
  TickCount.Reset(0);

  TD_Debugf("Select_Game: entering Main_Menu timeout=%lu", static_cast<unsigned long>(kMenuTimeoutMs));
  const int selection = Main_Menu(kMenuTimeoutMs);
  TD_Debugf("Select_Game: Main_Menu returned selection=%d", selection);
  if (selection < 0) {
    ReadyToQuit = true;
    return false;
  }

  const bool wants_scenario = (selection == 0 || selection == 1 || selection == 2 || selection == 4);
  switch (selection) {
    case 0:  // New missions (expansion)
      CarryOverMoney = 0;
      if (Expansion_Dialog()) {
        Theme.Fade_Out();
        GameToPlay = GAME_NORMAL;
      } else {
        return Select_Game(false);
      }
      break;
    case 1:  // Start new game
      CarryOverMoney = 0;
      Theme.Fade_Out();
      Configure_New_Game_From_Menu();
      GameToPlay = GAME_NORMAL;
      break;
    case 2:  // Bonus missions
      CarryOverMoney = 0;
      if (Bonus_Dialog()) {
        Theme.Fade_Out();
        GameToPlay = GAME_NORMAL;
      } else {
        return Select_Game(false);
      }
      break;
    case 3:  // Internet
      GameToPlay = GAME_INTERNET;
      break;
    case 4:  // Load mission
      if (LoadOptionsClass(LoadOptionsClass::LOAD).Process()) {
        Theme.Queue_Song(THEME_AOI);
        GameToPlay = GAME_NORMAL;
        gameloaded = true;
      } else {
        return Select_Game(false);
      }
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

  if (wants_scenario && !Debug_Map && !gameloaded) {

    Set_Scenario_Name(ScenarioName, Scenario, ScenPlayer, ScenDir);
    TD_Debugf("Select_Game: scenario root computed: %s", ScenarioName);

    Hide_Mouse();
    if (selection != 1) {
      Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    }
    HiddenPage.Clear();
    VisiblePage.Clear();
    Show_Mouse();

    CCDebugString("Starting scenario from main menu.\n");
    TD_Debugf("Select_Game: calling Start_Scenario(%s)", ScenarioName);
    if (!Start_Scenario(ScenarioName)) {
      CCDebugString("Start_Scenario failed.\n");
      TD_Debugf("Select_Game: Start_Scenario returned failure");
      return false;
    }
    TD_Debugf("Select_Game: Start_Scenario returned success");

    const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
    Messages.Init(Map.TacPixelX, Map.TacPixelY, 6, MAX_MESSAGE_LENGTH, 6 * factor + 1);

    Set_Logic_Page(SeenBuff);
    Map.Flag_To_Redraw(true);
    Call_Back();
    Map.Render();
  }

  return true;
}

#include "legacy/map.h"

bool Main_Loop() {
  static auto next_frame = std::chrono::steady_clock::now();
  static int last_frame_ms = 0;

  Configure_Game_Viewports();
  Pump_Sdl_Events();
  if (ReadyToQuit) {
    return true;
  }

  const int frame_ms = Target_Frame_Milliseconds();
  if (frame_ms != last_frame_ms) {
    next_frame = std::chrono::steady_clock::now();
    last_frame_ms = frame_ms;
  }

  if (!PlaybackGame && !GameInFocus) {
    Call_Back();
    SDL_Delay(50);
    next_frame = std::chrono::steady_clock::now();
    return !GameActive;
  }

  if (!PlaybackGame) {
    if (SpecialDialog == SDLG_NONE && GameInFocus) {
      if (WWMouse) {
        WWMouse->Erase_Mouse(&HidPage, true);
      }

      KeyNumType input = KN_NONE;
      int x = 0;
      int y = 0;
      Map.Input(input, x, y);
      if (input) {
        Keyboard_Process(input);
      }
      Map.Render();
    }
  }

  Map.Layer[LAYER_GROUND].Sort();
  Logic.AI();

  if (Messages.Manage()) {
    HiddenPage.Clear();
    Map.Flag_To_Redraw(true);
  }

  Queue_AI();

  Score.ElapsedTime += TIMER_SECOND / TICKS_PER_SECOND;
  Call_Back();

  if (EndCountDown) EndCountDown--;

  if (PlayerWins) {
    if (WWMouse) {
      WWMouse->Erase_Mouse(&HidPage, true);
    }
    PlayerLoses = false;
    PlayerWins = false;
    PlayerRestarts = false;
    Map.Help_Text(TXT_NONE);
    Do_Win();
  }

  if (PlayerLoses) {
    if (WWMouse) {
      WWMouse->Erase_Mouse(&HidPage, true);
    }
    PlayerWins = false;
    PlayerLoses = false;
    PlayerRestarts = false;
    Map.Help_Text(TXT_NONE);
    Do_Lose();
  }

  if (PlayerRestarts) {
    if (WWMouse) {
      WWMouse->Erase_Mouse(&HidPage, true);
    }
    PlayerWins = false;
    PlayerLoses = false;
    PlayerRestarts = false;
    Map.Help_Text(TXT_NONE);
    Do_Restart();
  }

  ++Frame;

  next_frame += std::chrono::milliseconds(frame_ms);
  const auto now = std::chrono::steady_clock::now();
  if (now < next_frame) {
    const auto sleep_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(next_frame - now).count();
    if (sleep_ms > 0) {
      SDL_Delay(static_cast<Uint32>(sleep_ms));
    }
  } else {
    // Avoid runaway catch-up after long stalls (debug breaks, minimize, etc).
    const auto behind_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - next_frame).count();
    if (behind_ms > 250) {
      next_frame = now;
    }
  }

  return !GameActive;
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
  int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

  int d_dialog_w = 170 * factor;
  int d_dialog_h = 53 * factor;
  int d_dialog_x = ((320 * factor - d_dialog_w) / 2);
  int d_dialog_y = ((200 * factor - d_dialog_h) / 2);
  int d_dialog_cx = d_dialog_x + (d_dialog_w / 2);

  int d_margin = 5 * factor;
  int d_topmargin = 20 * factor;

  int d_ok_w = 45 * factor;
  int d_ok_h = 9 * factor;
  int d_ok_x = d_dialog_cx - d_ok_w - 5 * factor;
  int d_ok_y = d_dialog_y + d_dialog_h - d_ok_h - d_margin;

  int d_cancel_w = 45 * factor;
  int d_cancel_h = 9 * factor;
  int d_cancel_x = d_dialog_cx + 5 * factor;
  int d_cancel_y = d_dialog_y + d_dialog_h - d_cancel_h - d_margin;

  enum { BUTTON_OK = 100, BUTTON_CANCEL };

  typedef enum {
    REDRAW_NONE = 0,
    REDRAW_BUTTONS,
    REDRAW_BACKGROUND,
    REDRAW_ALL = REDRAW_BACKGROUND
  } RedrawType;

  RedrawType display;
  bool process;
  KeyNumType input;
  int retcode;

  ControlClass* commands = nullptr;

  TextButtonClass okbtn(
      BUTTON_OK,
      TXT_OK,
      TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
      d_ok_x,
      d_ok_y,
      d_ok_w,
      d_ok_h);

  TextButtonClass cancelbtn(
      BUTTON_CANCEL,
      TXT_CANCEL,
      TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
      d_cancel_x,
      d_cancel_y,
      d_cancel_w,
      d_cancel_h);

  Set_Logic_Page(SeenBuff);

  commands = &okbtn;
  cancelbtn.Add_Tail(*commands);

  display = REDRAW_ALL;
  process = true;
  while (process) {
    if (AllSurfaces.SurfacesRestored) {
      AllSurfaces.SurfacesRestored = FALSE;
      display = REDRAW_ALL;
    }

    if (Main_Loop()) {
      retcode = 0;
      process = false;
    }

    if (display) {
      Hide_Mouse();
      if (display >= REDRAW_BACKGROUND) {
        Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
        Draw_Caption(TXT_NONE, d_dialog_x, d_dialog_y, d_dialog_w);

        Fancy_Text_Print(Text_String(TXT_SURRENDER),
                         d_dialog_cx,
                         d_dialog_y + d_topmargin,
                         CC_GREEN,
                         TBLACK,
                         TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                             TPF_NOSHADOW);
      }

      if (display >= REDRAW_BUTTONS) {
        commands->Flag_List_To_Redraw();
      }
      Show_Mouse();
      display = REDRAW_NONE;
    }

    input = commands->Input();

    switch (input) {
      case (KN_RETURN):
      case (BUTTON_OK | KN_BUTTON):
        retcode = 1;
        process = false;
        break;

      case (KN_ESC):
      case (BUTTON_CANCEL | KN_BUTTON):
        retcode = 0;
        process = false;
        break;

      default:
        break;
    }
  }

  HiddenPage.Clear();
  Map.Flag_To_Redraw(true);
  Map.Render();

  return retcode;
}

void Free_Scenario_Descriptions() {
  MPlayerScenarios.Clear();
  MPlayerFilenum.Clear();

  for (int i = 0; i < InitStrings.Count(); ++i) {
    delete InitStrings[i];
  }
  InitStrings.Clear();

  for (int i = 0; i < PhoneBook.Count(); ++i) {
    ::operator delete(static_cast<void*>(PhoneBook[i]));
  }
  PhoneBook.Clear();
}
