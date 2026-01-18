#include "legacy_compat.h"

#include "cell.h"
#include "sdl_platform.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <climits>
#include <string>
#include <vector>
#include <SDL.h>

 

void const* ObjectTypeClass::SelectShapes = NULL;
void const* ObjectTypeClass::PipShapes = NULL;

// Minimal globals to satisfy legacy references until full implementations
// are ported.
#include "map_fwd.h"

DynamicVectorClass<TriggerClass*> CellTriggers;

void Hide_Mouse() { SDL_ShowCursor(SDL_DISABLE); }
void Show_Mouse() { SDL_ShowCursor(SDL_ENABLE); }

int Get_Mouse_X() { return SDL_Platform_Mouse_X(); }
int Get_Mouse_Y() { return SDL_Platform_Mouse_Y(); }

int Desired_Facing8(int center_x, int center_y, int mouse_x, int mouse_y)
{
    const int dx = mouse_x - center_x;
    const int dy = mouse_y - center_y;
    if (dx == 0 && dy == 0) return FACING_NONE;

    // Screen coordinates: +y is down. Map to 8-way facing.
    // Angle 0 points east, then clockwise.
    double angle = std::atan2((double)dy, (double)dx);  // [-pi, pi]
    const double pi = 3.14159265358979323846;
    if (angle < 0.0) angle += 2.0 * pi;
    int octant = (int)((angle / (2.0 * pi)) * 8.0 + 0.5) & 7;
    static const int facing_map[8] = {FACING_E, FACING_SE, FACING_S, FACING_SW, FACING_W, FACING_NW, FACING_N, FACING_NE};
    return facing_map[octant];
}

void Sticky_Process(unsigned) {}

COORDINATE Coord_Add(COORDINATE coord1, COORDINATE coord2) { return coord1 + coord2; }

namespace {
static bool g_mouse_hidden_conditionally = false;
}

void Conditional_Hide_Mouse(int x, int y, int w, int h)
{
    const int mx = Get_Mouse_X();
    const int my = Get_Mouse_Y();
    if (w <= x || h <= y) return;
    const bool inside = (mx >= x && my >= y && mx < w && my < h);
    if (inside && !g_mouse_hidden_conditionally) {
        Hide_Mouse();
        g_mouse_hidden_conditionally = true;
    }
}

void Conditional_Show_Mouse(void)
{
    if (g_mouse_hidden_conditionally) {
        Show_Mouse();
        g_mouse_hidden_conditionally = false;
    }
}

int String_Pixel_Width(char const* text) {
    if (!text) return 0;
    return static_cast<int>(std::strlen(text)) * 6;
}

int Char_Pixel_Width(char character) {
    (void)character;
    return 6;
}

void Set_Font(void const*) {}

void Set_Font_Palette(unsigned char const[16]) {}

char const* Text_String(int text_id) {
    switch (text_id) {
        case TXT_YES:
            return "Yes";
        case TXT_NO:
            return "No";
        case TXT_OK:
            return "OK";
        case TXT_CANCEL:
            return "Cancel";
        case TXT_START_NEW_GAME:
            return "Start New Game";
        case TXT_LOAD_MISSION:
            return "Load Mission";
        case TXT_OPTIONS:
            return "Options";
        case TXT_EXIT_GAME:
            return "Exit Game";
        case TXT_INTRO:
            return "Intro & Sneak Peek";
        case TXT_CONFIRMATION:
            return "Confirmation";
        case TXT_NONE:
        default:
            return "";
    }
}

void Draw_Caption(int text, int x, int y, int w)
{
    if (!LogicPage) return;
    const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
    const int caption_h = (FontHeight + FontYSpacing + 6) * factor;

    LogicPage->Fill_Rect(x + 2, y + 2, x + w - 3, y + caption_h - 3, CC_GREEN_BKGD);
    LogicPage->Draw_Rect(x + 1, y + 1, x + w - 2, y + caption_h - 2, 14);
    LogicPage->Draw_Rect(x, y, x + w - 1, y + caption_h - 1, 13);

    char const* caption = Text_String(text);
    if (!caption) caption = "";
    const int text_w = String_Pixel_Width(caption);
    const int text_x = x + (w - text_w) / 2;
    const int text_y = y + 3 * factor;
    LogicPage->Print(caption, text_x, text_y, WHITE, TBLACK);
}

void CC_Texture_Fill(void const* shapefile, int shapenum, int xpos, int ypos, int width, int height)
{
    (void)shapefile;
    (void)shapenum;
    if (!LogicPage) return;
    if (width <= 0 || height <= 0) return;

    LogicPage->Fill_Rect(xpos, ypos, xpos + width - 1, ypos + height - 1, CC_GREEN_BKGD);

    // Simple dither overlay to avoid a flat fill until SHP texture decoding lands.
    for (int y = ypos; y < ypos + height; ++y) {
        for (int x = xpos + ((y & 1) ? 1 : 0); x < xpos + width; x += 2) {
            LogicPage->Put_Pixel(x, y, 12);
        }
    }
}


void CCDebugString(char const* string) {
    if (!string) return;
    std::fputs(string, stderr);
    std::fflush(stderr);
}

CellClass& MapClass::operator[](CELL) { static CellClass dummy; return dummy; }

CellClass const& MapClass::operator[](CELL) const { static CellClass dummy; return dummy; }

namespace {
static short const SmudgeEmptyList[] = {REFRESH_EOL};
}  // namespace

SmudgeTypeClass const& SmudgeTypeClass::As_Reference(SmudgeType) {
    static SmudgeTypeClass dummy;
    return dummy;
}

short const* SmudgeTypeClass::Occupy_List() const { return SmudgeEmptyList; }
short const* SmudgeTypeClass::Overlap_List() const { return SmudgeEmptyList; }

namespace {
struct IniCursor {
    char const* Ptr;
    explicit IniCursor(char const* ptr) : Ptr(ptr ? ptr : "") {}
};

static void Skip_Spaces(IniCursor& cursor) {
    while (*cursor.Ptr == ' ' || *cursor.Ptr == '\t' || *cursor.Ptr == '\r') ++cursor.Ptr;
}

static bool Read_Line(IniCursor& cursor, std::string& line) {
    line.clear();
    if (!cursor.Ptr || !*cursor.Ptr) return false;
    while (*cursor.Ptr && *cursor.Ptr != '\n') {
        line.push_back(*cursor.Ptr);
        ++cursor.Ptr;
    }
    if (*cursor.Ptr == '\n') ++cursor.Ptr;
    return true;
}

static bool Parse_Section(std::string const& line, std::string& out_section) {
    if (line.size() < 2) return false;
    if (line[0] != '[') return false;
    size_t close = line.find(']');
    if (close == std::string::npos || close <= 1) return false;
    out_section = line.substr(1, close - 1);
    return true;
}

static bool Split_Key_Value(std::string const& line, std::string& out_key, std::string& out_value) {
    size_t eq = line.find('=');
    if (eq == std::string::npos) return false;
    out_key = line.substr(0, eq);
    out_value = line.substr(eq + 1);
    return true;
}

static void Trim(std::string& value) {
    while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
    while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t' ||
                              value[value.size() - 1] == '\r')) {
        value.erase(value.size() - 1);
    }
}

static bool Equals_CI(std::string const& a, char const* b) {
    if (!b) return a.empty();
    return strcasecmp(a.c_str(), b) == 0;
}

static bool Find_Ini_Value(char const* section, char const* key, char const* buffer, std::string& out_value) {
    if (!buffer || !section || !key) return false;
    IniCursor cursor(buffer);
    std::string line;
    std::string current_section;
    bool in_section = false;

    while (Read_Line(cursor, line)) {
        if (!line.empty() && (line[0] == ';' || line[0] == '#')) continue;

        std::string found_section;
        if (Parse_Section(line, found_section)) {
            Trim(found_section);
            current_section = found_section;
            in_section = Equals_CI(current_section, section);
            continue;
        }

        if (!in_section) continue;

        std::string parsed_key;
        std::string parsed_value;
        if (!Split_Key_Value(line, parsed_key, parsed_value)) continue;
        Trim(parsed_key);
        Trim(parsed_value);
        if (!Equals_CI(parsed_key, key)) continue;

        out_value = parsed_value;
        return true;
    }
    return false;
}
}  // namespace

int WWGetPrivateProfileInt(char const* section, char const* key, int default_value, char const* buffer) {
    std::string value;
    if (!Find_Ini_Value(section, key, buffer, value)) return default_value;
    return std::atoi(value.c_str());
}

int WWGetPrivateProfileString(char const* section,
                              char const* key,
                              char const* default_value,
                              char* output,
                              int output_len,
                              char const* buffer) {
    if (!output || output_len <= 0) return 0;

    std::string value;
    if (!Find_Ini_Value(section, key, buffer, value)) {
        value = default_value ? default_value : "";
    }

    ::snprintf(output, static_cast<size_t>(output_len), "%s", value.c_str());
    output[output_len - 1] = '\0';
    return static_cast<int>(std::strlen(output));
}

bool WWWritePrivateProfileString(char const*, char const*, char const*, char*) { return true; }
bool WWWritePrivateProfileInt(char const*, char const*, int, char*) { return true; }

TheaterType LastTheater = THEATER_NONE;

COORDINATE As_Coord(TARGET target) { return static_cast<COORDINATE>(target); }

BuildingClass* As_Building(TARGET) { return NULL; }

DirType Direction(COORDINATE coord1, COORDINATE coord2) {
    (void)coord1;
    (void)coord2;
    return FACING_NONE;
}

int Distance(COORDINATE coord1, COORDINATE coord2) {
    COORDINATE diff = (coord2 > coord1) ? (coord2 - coord1) : (coord1 - coord2);
    if (diff > static_cast<COORDINATE>(INT_MAX)) return INT_MAX;
    return static_cast<int>(diff);
}

int Get_Resolution_Factor(void) {
    return 0;
}
