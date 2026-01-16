#include "legacy_compat.h"

#include "cell.h"
#include "sdl_platform.h"

#include <cstdlib>
#include <cstdio>
#include <climits>
#include <string>
#include <vector>
#include <SDL.h>

namespace {
static unsigned char const Font8x8[128][8] = {
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}, {24,60,60,24,24,0,24,0}, {102,102,36,0,0,0,0,0},
    {54,54,127,54,127,54,54,0}, {24,62,96,60,6,124,24,0}, {0,99,102,12,24,51,99,0},
    {28,54,28,59,102,102,59,0}, {12,12,24,0,0,0,0,0}, {6,12,24,24,24,12,6,0},
    {48,24,12,12,12,24,48,0}, {0,102,60,255,60,102,0,0}, {0,24,24,126,24,24,0,0},
    {0,0,0,0,0,12,12,24}, {0,0,0,126,0,0,0,0}, {0,0,0,0,0,24,24,0},
    {3,6,12,24,48,96,192,0}, {62,99,103,111,123,115,62,0}, {24,56,24,24,24,24,126,0},
    {62,99,3,6,24,48,127,0}, {62,99,3,30,3,99,62,0}, {6,14,30,54,127,6,6,0},
    {127,96,124,3,3,99,62,0}, {30,48,96,124,99,99,62,0}, {127,99,6,12,24,24,24,0},
    {62,99,99,62,99,99,62,0}, {62,99,99,63,3,6,60,0}, {0,24,24,0,0,24,24,0},
    {0,24,24,0,0,24,24,48}, {6,12,24,48,24,12,6,0}, {0,0,126,0,126,0,0,0},
    {48,24,12,6,12,24,48,0}, {62,99,6,12,24,0,24,0},
    {62,99,123,123,123,96,62,0},
    {24,60,102,102,126,102,102,0}, {124,102,102,124,102,102,124,0}, {62,99,96,96,96,99,62,0},
    {124,102,99,99,99,102,124,0}, {127,96,96,124,96,96,127,0}, {127,96,96,124,96,96,96,0},
    {62,99,96,96,111,99,62,0}, {99,99,99,127,99,99,99,0}, {126,24,24,24,24,24,126,0},
    {31,6,6,6,6,102,60,0}, {99,102,108,120,108,102,99,0}, {96,96,96,96,96,96,127,0},
    {99,119,127,107,99,99,99,0}, {99,115,123,111,103,99,99,0}, {62,99,99,99,99,99,62,0},
    {124,102,102,124,96,96,96,0}, {62,99,99,99,107,102,59,0}, {124,102,102,124,108,102,99,0},
    {62,99,96,62,3,99,62,0}, {255,24,24,24,24,24,24,0}, {99,99,99,99,99,99,62,0},
    {99,99,99,99,99,54,28,0}, {99,99,99,107,127,119,99,0}, {99,99,54,28,54,99,99,0},
    {102,102,102,60,24,24,24,0}, {127,3,6,12,24,48,127,0},
    {30,24,24,24,24,24,30,0}, {192,96,48,24,12,6,3,0}, {30,6,6,6,6,6,30,0},
    {8,28,54,99,0,0,0,0}, {0,0,0,0,0,0,0,255}, {24,24,12,0,0,0,0,0},
    {0,0,60,6,62,102,59,0}, {96,96,124,102,102,102,124,0}, {0,0,62,99,96,99,62,0},
    {3,3,63,99,99,99,63,0}, {0,0,62,99,127,96,62,0}, {14,27,24,124,24,24,24,0},
    {0,0,63,99,99,63,3,62}, {96,96,124,102,102,102,102,0}, {24,0,56,24,24,24,60,0},
    {6,0,6,6,6,6,102,60}, {96,96,102,108,120,108,102,0}, {56,24,24,24,24,24,60,0},
    {0,0,118,127,107,99,99,0}, {0,0,124,102,102,102,102,0}, {0,0,62,99,99,99,62,0},
    {0,0,124,102,102,124,96,96}, {0,0,63,99,99,63,3,3}, {0,0,109,118,96,96,96,0},
    {0,0,63,96,62,3,126,0}, {24,24,126,24,24,27,14,0}, {0,0,102,102,102,102,59,0},
    {0,0,102,102,102,60,24,0}, {0,0,99,99,107,127,54,0}, {0,0,99,54,28,54,99,0},
    {0,0,102,102,102,62,6,124}, {0,0,127,6,24,96,127,0},
    {14,24,24,112,24,24,14,0}, {24,24,24,0,24,24,24,0}, {112,24,24,14,24,24,112,0},
    {50,76,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}
};

static void Put_Pixel_Buffer(unsigned char* dst, int width, int height, int pitch, int x, int y, unsigned char color)
{
    if (!dst) return;
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    dst[y * pitch + x] = color;
}

static void Draw_Char_8x8(unsigned char* dst, int width, int height, int pitch, int x, int y, unsigned char fore, unsigned char back, unsigned char c)
{
    if (!dst) return;
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = Font8x8[c][row];
        for (int col = 0; col < 8; ++col) {
            unsigned char color = (bits & (1u << (7 - col))) ? fore : back;
            Put_Pixel_Buffer(dst, width, height, pitch, x + col, y + row, color);
        }
    }
}
}  // namespace

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
    const bool inside = (mx >= x && my >= y && mx < x + w && my < y + h);
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

    LogicPage->Fill_Rect(x + 2, y + 2, w - 4, caption_h - 4, CC_GREEN_BKGD);
    LogicPage->Draw_Rect(x + 1, y + 1, w - 2, caption_h - 2, 14);
    LogicPage->Draw_Rect(x, y, w, caption_h, 13);

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

    LogicPage->Fill_Rect(xpos, ypos, width, height, CC_GREEN_BKGD);

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

GraphicPageClass::GraphicPageClass() {}
GraphicPageClass::~GraphicPageClass() {}
void GraphicPageClass::Draw_Rect(int x, int y, int w, int h, int color)
{
    Fill_Rect(x, y, w, 1, color);
    Fill_Rect(x, y + h - 1, w, 1, color);
    Fill_Rect(x, y, 1, h, color);
    Fill_Rect(x + w - 1, y, 1, h, color);
}

void GraphicPageClass::Fill_Rect(int x, int y, int w, int h, int color)
{
    GraphicBufferClass* buffer = dynamic_cast<GraphicBufferClass*>(this);
    if (!buffer) return;
    unsigned char* dst = buffer->Data();
    int pitch = buffer->Pitch();
    int width = buffer->Get_Width();
    int height = buffer->Get_Height();
    if (!dst) return;

    if (w <= 0 || h <= 0) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;
    if (w <= 0 || h <= 0) return;

    unsigned char c = (unsigned char)(color & 0xff);
    for (int row = 0; row < h; ++row) {
        std::memset(dst + (y + row) * pitch + x, c, (size_t)w);
    }
}

void GraphicPageClass::Draw_Line(int x1, int y1, int x2, int y2, int color)
{
    GraphicBufferClass* buffer = dynamic_cast<GraphicBufferClass*>(this);
    if (!buffer) return;
    unsigned char* dst = buffer->Data();
    int pitch = buffer->Pitch();
    int width = buffer->Get_Width();
    int height = buffer->Get_Height();
    unsigned char c = (unsigned char)(color & 0xff);

    int dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    int sx = (x1 < x2) ? 1 : -1;
    int dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    int sy = (y1 < y2) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    for (;;) {
        Put_Pixel_Buffer(dst, width, height, pitch, x1, y1, c);
        if (x1 == x2 && y1 == y2) break;
        int e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
}

void GraphicPageClass::Put_Pixel(int x, int y, int color)
{
    GraphicBufferClass* buffer = dynamic_cast<GraphicBufferClass*>(this);
    if (!buffer) return;
    Put_Pixel_Buffer(buffer->Data(), buffer->Get_Width(), buffer->Get_Height(), buffer->Pitch(), x, y,
                     (unsigned char)(color & 0xff));
}

void GraphicPageClass::Print(char const* text, int x, int y, int fore, int back)
{
    GraphicBufferClass* buffer = dynamic_cast<GraphicBufferClass*>(this);
    if (!buffer) return;
    if (!text) return;

    unsigned char* dst = buffer->Data();
    int pitch = buffer->Pitch();
    int width = buffer->Get_Width();
    int height = buffer->Get_Height();

    unsigned char f = (unsigned char)(fore & 0xff);
    unsigned char b = (unsigned char)(back & 0xff);

    int cursor_x = x;
    for (char const* p = text; *p; ++p) {
        if (*p == '\r' || *p == '\n') {
            cursor_x = x;
            y += 8 + FontYSpacing;
            continue;
        }
        unsigned char ch = (unsigned char)*p;
        Draw_Char_8x8(dst, width, height, pitch, cursor_x, y, f, b, ch);
        cursor_x += 8 + FontXSpacing;
    }
}

void GraphicPageClass::Clear()
{
    GraphicBufferClass* buffer = dynamic_cast<GraphicBufferClass*>(this);
    if (!buffer) return;
    unsigned char* dst = buffer->Data();
    if (!dst) return;
    std::memset(dst, 0, (size_t)buffer->Pitch() * (size_t)buffer->Get_Height());
}

bool GraphicPageClass::Lock() { return true; }
void GraphicPageClass::Unlock() {}

unsigned char* GraphicBufferClass::Data()
{
    if (Pixels.empty()) {
        Pixels.resize((size_t)Pitch() * (size_t)Height);
        std::memset(&Pixels[0], 0, Pixels.size());
    }
    return Pixels.empty() ? 0 : &Pixels[0];
}

unsigned char const* GraphicBufferClass::Data() const
{
    return Pixels.empty() ? 0 : &Pixels[0];
}

int GraphicBufferClass::Pitch() const { return Width; }
