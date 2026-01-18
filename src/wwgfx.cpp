#include "wwgfx.h"

#include <algorithm>
#include <cstring>

extern int FontXSpacing;
extern int FontYSpacing;

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

struct SurfaceView {
    unsigned char* base;
    unsigned char* pixels;
    int pitch;
    int w;
    int h;
};

static SurfaceView Make_View(GraphicBufferClass const& buffer)
{
    SurfaceView view;
    view.base = (unsigned char*)buffer.Get_Buffer();
    view.pixels = (unsigned char*)buffer.Get_Offset();
    view.pitch = buffer.Get_Pitch();
    view.w = buffer.Get_Width();
    view.h = buffer.Get_Height();
    return view;
}

static SurfaceView Make_View(GraphicBufferClass& buffer)
{
    SurfaceView view;
    view.base = (unsigned char*)buffer.Get_Buffer();
    view.pixels = (unsigned char*)buffer.Get_Offset();
    view.pitch = buffer.Get_Pitch();
    view.w = buffer.Get_Width();
    view.h = buffer.Get_Height();
    return view;
}

static SurfaceView Make_View(GraphicViewPortClass const& view_port)
{
    SurfaceView view;
    GraphicBufferClass const* buffer = view_port.Get_Graphic_Buffer();
    if (!buffer) {
        view.base = 0;
        view.pixels = 0;
        view.pitch = 0;
        view.w = 0;
        view.h = 0;
        return view;
    }
    view.base = (unsigned char*)buffer->Get_Buffer();
    view.pixels = (unsigned char*)view_port.Get_Offset();
    view.pitch = buffer->Get_Pitch();
    view.w = view_port.Get_Width();
    view.h = view_port.Get_Height();
    return view;
}

static SurfaceView Make_View(GraphicViewPortClass& view_port)
{
    SurfaceView view;
    GraphicBufferClass* buffer = view_port.Get_Graphic_Buffer();
    if (!buffer) {
        view.base = 0;
        view.pixels = 0;
        view.pitch = 0;
        view.w = 0;
        view.h = 0;
        return view;
    }
    view.base = (unsigned char*)buffer->Get_Buffer();
    view.pixels = (unsigned char*)view_port.Get_Offset();
    view.pitch = buffer->Get_Pitch();
    view.w = view_port.Get_Width();
    view.h = view_port.Get_Height();
    return view;
}

static void Put_Pixel_View(SurfaceView const& view, int x, int y, unsigned char color)
{
    if (!view.pixels) return;
    if (x < 0 || y < 0 || x >= view.w || y >= view.h) return;
    view.pixels[y * view.pitch + x] = color;
}

static unsigned char Get_Pixel_View(SurfaceView const& view, int x, int y)
{
    if (!view.pixels) return 0;
    if (x < 0 || y < 0 || x >= view.w || y >= view.h) return 0;
    return view.pixels[y * view.pitch + x];
}

static void Fill_Rect_View(SurfaceView const& view, int x1, int y1, int x2, int y2, unsigned char color)
{
    if (!view.pixels) return;
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    x1 = std::max(x1, 0);
    y1 = std::max(y1, 0);
    x2 = std::min(x2, view.w - 1);
    y2 = std::min(y2, view.h - 1);
    if (x2 < x1 || y2 < y1) return;

    const int row_bytes = x2 - x1 + 1;
    for (int y = y1; y <= y2; ++y) {
        std::memset(view.pixels + y * view.pitch + x1, (int)color, (size_t)row_bytes);
    }
}

static void Draw_Line_View(SurfaceView const& view, int x1, int y1, int x2, int y2, unsigned char color)
{
    if (!view.pixels) return;

    int dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    int sx = (x1 < x2) ? 1 : -1;
    int dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    int sy = (y1 < y2) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    for (;;) {
        Put_Pixel_View(view, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

static void Draw_Rect_View(SurfaceView const& view, int x1, int y1, int x2, int y2, unsigned char color)
{
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    Draw_Line_View(view, x1, y1, x2, y1, color);
    Draw_Line_View(view, x1, y2, x2, y2, color);
    Draw_Line_View(view, x1, y1, x1, y2, color);
    Draw_Line_View(view, x2, y1, x2, y2, color);
}

static void Draw_Char_8x8(SurfaceView const& view, int x, int y, unsigned char fore, unsigned char back, unsigned char c)
{
    if (!view.pixels) return;
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = Font8x8[c][row];
        for (int col = 0; col < 8; ++col) {
            unsigned char color = (bits & (1u << (7 - col))) ? fore : back;
            Put_Pixel_View(view, x + col, y + row, color);
        }
    }
}

static void Print_View(SurfaceView const& view, char const* text, int x, int y, unsigned char fore, unsigned char back)
{
    if (!text) return;
    int cursor_x = x;
    for (char const* p = text; *p; ++p) {
        if (*p == '\r' || *p == '\n') {
            cursor_x = x;
            y += 8 + FontYSpacing;
            continue;
        }
        unsigned char ch = (unsigned char)*p;
        Draw_Char_8x8(view, cursor_x, y, fore, back, ch);
        cursor_x += 8 + FontXSpacing;
    }
}

static void Blit_View(SurfaceView const& src,
                      SurfaceView const& dst,
                      int src_x,
                      int src_y,
                      int dst_x,
                      int dst_y,
                      int width,
                      int height,
                      bool use_key)
{
    if (!src.pixels || !dst.pixels) return;
    if (width <= 0 || height <= 0) return;

    if (src_x < 0) { dst_x -= src_x; width += src_x; src_x = 0; }
    if (src_y < 0) { dst_y -= src_y; height += src_y; src_y = 0; }
    if (dst_x < 0) { src_x -= dst_x; width += dst_x; dst_x = 0; }
    if (dst_y < 0) { src_y -= dst_y; height += dst_y; dst_y = 0; }

    if (src_x + width > src.w) width = src.w - src_x;
    if (src_y + height > src.h) height = src.h - src_y;
    if (dst_x + width > dst.w) width = dst.w - dst_x;
    if (dst_y + height > dst.h) height = dst.h - dst_y;
    if (width <= 0 || height <= 0) return;

    const bool same_surface = (src.base && (src.base == dst.base));
    const bool copy_bottom_up = same_surface && (dst.pixels > src.pixels) && (dst_y > src_y);

    if (!use_key) {
        if (copy_bottom_up) {
            for (int row = height - 1; row >= 0; --row) {
                unsigned char const* src_row = src.pixels + (src_y + row) * src.pitch + src_x;
                unsigned char* dst_row = dst.pixels + (dst_y + row) * dst.pitch + dst_x;
                std::memmove(dst_row, src_row, (size_t)width);
            }
        } else {
            for (int row = 0; row < height; ++row) {
                unsigned char const* src_row = src.pixels + (src_y + row) * src.pitch + src_x;
                unsigned char* dst_row = dst.pixels + (dst_y + row) * dst.pitch + dst_x;
                std::memmove(dst_row, src_row, (size_t)width);
            }
        }
        return;
    }

    if (copy_bottom_up) {
        for (int row = height - 1; row >= 0; --row) {
            unsigned char const* src_row = src.pixels + (src_y + row) * src.pitch + src_x;
            unsigned char* dst_row = dst.pixels + (dst_y + row) * dst.pitch + dst_x;
            for (int col = 0; col < width; ++col) {
                unsigned char c = src_row[col];
                if (c) dst_row[col] = c;
            }
        }
    } else {
        for (int row = 0; row < height; ++row) {
            unsigned char const* src_row = src.pixels + (src_y + row) * src.pitch + src_x;
            unsigned char* dst_row = dst.pixels + (dst_y + row) * dst.pitch + dst_x;
            for (int col = 0; col < width; ++col) {
                unsigned char c = src_row[col];
                if (c) dst_row[col] = c;
            }
        }
    }
}

static void Scale_View(SurfaceView const& src,
                       SurfaceView const& dst,
                       int src_x,
                       int src_y,
                       int dst_x,
                       int dst_y,
                       int src_w,
                       int src_h,
                       int dst_w,
                       int dst_h,
                       bool use_key,
                       unsigned char const* remap_table)
{
    if (!src.pixels || !dst.pixels) return;
    if (src_w <= 0 || src_h <= 0 || dst_w <= 0 || dst_h <= 0) return;

    for (int dy = 0; dy < dst_h; ++dy) {
        const int out_y = dst_y + dy;
        if (out_y < 0 || out_y >= dst.h) continue;

        const int in_y = src_y + (dy * src_h) / dst_h;
        if (in_y < 0 || in_y >= src.h) continue;

        unsigned char const* src_row = src.pixels + in_y * src.pitch;
        unsigned char* dst_row = dst.pixels + out_y * dst.pitch;

        for (int dx = 0; dx < dst_w; ++dx) {
            const int out_x = dst_x + dx;
            if (out_x < 0 || out_x >= dst.w) continue;

            const int in_x = src_x + (dx * src_w) / dst_w;
            if (in_x < 0 || in_x >= src.w) continue;

            unsigned char c = src_row[in_x];
            if (use_key && c == 0) continue;
            if (remap_table) c = remap_table[c];
            dst_row[out_x] = c;
        }
    }
}
}  // namespace

GraphicBufferClass::GraphicBufferClass()
    : Width(0), Height(0), PitchBytes(0), LockCount(0), Pixels(0), OwnsPixels(false), OwnedSize(0)
{
}

GraphicBufferClass::GraphicBufferClass(int width, int height)
    : Width(0), Height(0), PitchBytes(0), LockCount(0), Pixels(0), OwnsPixels(false), OwnedSize(0)
{
    Init(width, height, 0, 0, 0);
}

GraphicBufferClass::GraphicBufferClass(int width, int height, void* buffer)
    : Width(0), Height(0), PitchBytes(0), LockCount(0), Pixels(0), OwnsPixels(false), OwnedSize(0)
{
    Init(width, height, buffer, 0, 0);
}

GraphicBufferClass::GraphicBufferClass(int width, int height, void* buffer, long buffer_size)
    : Width(0), Height(0), PitchBytes(0), LockCount(0), Pixels(0), OwnsPixels(false), OwnedSize(0)
{
    Init(width, height, buffer, buffer_size, 0);
}

GraphicBufferClass::~GraphicBufferClass()
{
    Un_Init();
}

bool GraphicBufferClass::Init(int width, int height, void* buffer, long buffer_size, int)
{
    Un_Init();

    Width = (width > 0) ? width : 0;
    Height = (height > 0) ? height : 0;
    PitchBytes = Width;
    LockCount = 0;

    const size_t required = (size_t)PitchBytes * (size_t)Height;
    if (required == 0) return true;

    if (buffer && buffer_size > 0 && (size_t)buffer_size >= required) {
        Pixels = (unsigned char*)buffer;
        OwnsPixels = false;
        OwnedSize = 0;
        return true;
    }

    Pixels = new unsigned char[required];
    OwnsPixels = true;
    OwnedSize = required;
    std::memset(Pixels, 0, required);
    return true;
}

void GraphicBufferClass::Un_Init()
{
    if (OwnsPixels && Pixels) {
        delete[] Pixels;
    }
    Width = 0;
    Height = 0;
    PitchBytes = 0;
    LockCount = 0;
    Pixels = 0;
    OwnsPixels = false;
    OwnedSize = 0;
}

int GraphicBufferClass::Get_Width() const { return Width; }
int GraphicBufferClass::Get_Height() const { return Height; }
int GraphicBufferClass::Get_Pitch() const { return PitchBytes; }
int GraphicBufferClass::Get_XAdd() const { return PitchBytes - Width; }
int GraphicBufferClass::Get_LockCount() const { return LockCount; }
bool GraphicBufferClass::Get_IsDirectDraw() const { return false; }

void* GraphicBufferClass::Get_Buffer() { return Pixels; }
void const* GraphicBufferClass::Get_Buffer() const { return Pixels; }
void* GraphicBufferClass::Get_Offset() { return Pixels; }
void const* GraphicBufferClass::Get_Offset() const { return Pixels; }

unsigned char* GraphicBufferClass::Data() { return (unsigned char*)Get_Offset(); }
unsigned char const* GraphicBufferClass::Data() const { return (unsigned char const*)Get_Offset(); }
int GraphicBufferClass::Pitch() const { return Get_Pitch(); }

bool GraphicBufferClass::Lock()
{
    if (!Pixels) return false;
    ++LockCount;
    return true;
}

void GraphicBufferClass::Unlock()
{
    if (LockCount > 0) --LockCount;
}

void GraphicBufferClass::Clear(int color)
{
    if (!Pixels) return;
    const size_t required = (size_t)PitchBytes * (size_t)Height;
    std::memset(Pixels, color & 0xff, required);
}

int GraphicBufferClass::Get_Pixel(int x, int y) const
{
    SurfaceView view = Make_View(*this);
    return (int)Get_Pixel_View(view, x, y);
}

void GraphicBufferClass::Put_Pixel(int x, int y, int color)
{
    SurfaceView view = Make_View(*this);
    Put_Pixel_View(view, x, y, (unsigned char)(color & 0xff));
}

void GraphicBufferClass::Fill_Rect(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Fill_Rect_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicBufferClass::Draw_Rect(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Draw_Rect_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicBufferClass::Draw_Line(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Draw_Line_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicBufferClass::Print(char const* text, int x, int y, int fore, int back)
{
    SurfaceView view = Make_View(*this);
    Print_View(view, text, x, y, (unsigned char)(fore & 0xff), (unsigned char)(back & 0xff));
}

void GraphicBufferClass::Blit(GraphicBufferClass& dest)
{
    Blit(dest, 0, 0);
}

void GraphicBufferClass::Blit(GraphicBufferClass& dest, int dest_x, int dest_y)
{
    Blit(dest, 0, 0, dest_x, dest_y, Get_Width(), Get_Height(), false);
}

void GraphicBufferClass::Blit(GraphicBufferClass& dest,
                              int src_x,
                              int src_y,
                              int dest_x,
                              int dest_y,
                              int width,
                              int height,
                              bool use_key)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Blit_View(src, dst, src_x, src_y, dest_x, dest_y, width, height, use_key);
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest)
{
    Blit(dest, 0, 0);
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest, int dest_x, int dest_y)
{
    Blit(dest, 0, 0, dest_x, dest_y, Get_Width(), Get_Height(), false);
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest,
                              int src_x,
                              int src_y,
                              int dest_x,
                              int dest_y,
                              int width,
                              int height,
                              bool use_key)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Blit_View(src, dst, src_x, src_y, dest_x, dest_y, width, height, use_key);
}

void GraphicBufferClass::Scale(GraphicViewPortClass& dest,
                               int src_x,
                               int src_y,
                               int dest_x,
                               int dest_y,
                               int src_w,
                               int src_h,
                               int dest_w,
                               int dest_h)
{
    Scale(dest, src_x, src_y, dest_x, dest_y, src_w, src_h, dest_w, dest_h, false, 0);
}

void GraphicBufferClass::Scale(GraphicViewPortClass& dest,
                               int src_x,
                               int src_y,
                               int dest_x,
                               int dest_y,
                               int src_w,
                               int src_h,
                               int dest_w,
                               int dest_h,
                               bool use_key)
{
    Scale(dest, src_x, src_y, dest_x, dest_y, src_w, src_h, dest_w, dest_h, use_key, 0);
}

void GraphicBufferClass::Scale(GraphicViewPortClass& dest,
                               int src_x,
                               int src_y,
                               int dest_x,
                               int dest_y,
                               int src_w,
                               int src_h,
                               int dest_w,
                               int dest_h,
                               bool use_key,
                               char const* remap_table)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Scale_View(src,
               dst,
               src_x,
               src_y,
               dest_x,
               dest_y,
               src_w,
               src_h,
               dest_w,
               dest_h,
               use_key,
               (unsigned char const*)remap_table);
}

int GraphicBufferClass::To_Buffer(int x, int y, int width, int height, void* out, int out_bytes) const
{
    if (!out) return 0;
    if (width <= 0 || height <= 0) return 0;
    const int required = width * height;
    if (out_bytes < required) return 0;

    SurfaceView src = Make_View(*this);
    unsigned char* dst = (unsigned char*)out;

    for (int row = 0; row < height; ++row) {
        const int sy = y + row;
        for (int col = 0; col < width; ++col) {
            const int sx = x + col;
            dst[row * width + col] = Get_Pixel_View(src, sx, sy);
        }
    }
    return required;
}

void GraphicBufferClass::Attach_DD_Surface(GraphicBufferClass*)
{
}

void* GraphicBufferClass::Get_DD_Surface() const
{
    return 0;
}

GraphicViewPortClass::GraphicViewPortClass() : Buffer(0), X(0), Y(0), Width(0), Height(0) {}

GraphicViewPortClass::GraphicViewPortClass(GraphicBufferClass* buffer, int x, int y, int width, int height)
    : Buffer(0), X(0), Y(0), Width(0), Height(0)
{
    Attach(buffer, x, y, width, height);
}

void GraphicViewPortClass::Attach(GraphicBufferClass* buffer, int x, int y, int width, int height)
{
    Buffer = buffer;
    X = x;
    Y = y;
    Width = (width > 0) ? width : 0;
    Height = (height > 0) ? height : 0;
}

GraphicBufferClass* GraphicViewPortClass::Get_Graphic_Buffer() { return Buffer; }
GraphicBufferClass const* GraphicViewPortClass::Get_Graphic_Buffer() const { return Buffer; }

int GraphicViewPortClass::Get_XPos() const { return X; }
int GraphicViewPortClass::Get_YPos() const { return Y; }
int GraphicViewPortClass::Get_Width() const { return Width; }
int GraphicViewPortClass::Get_Height() const { return Height; }
int GraphicViewPortClass::Get_Pitch() const { return Buffer ? Buffer->Get_Pitch() : 0; }
int GraphicViewPortClass::Get_XAdd() const { return Buffer ? (Buffer->Get_Pitch() - Width) : 0; }
int GraphicViewPortClass::Get_LockCount() const { return Buffer ? Buffer->Get_LockCount() : 0; }
bool GraphicViewPortClass::Get_IsDirectDraw() const { return Buffer ? Buffer->Get_IsDirectDraw() : false; }

void* GraphicViewPortClass::Get_Offset()
{
    if (!Buffer) return 0;
    unsigned char* base = Buffer->Data();
    return base ? (void*)(base + Y * Buffer->Get_Pitch() + X) : 0;
}

void const* GraphicViewPortClass::Get_Offset() const
{
    if (!Buffer) return 0;
    unsigned char const* base = Buffer->Data();
    return base ? (void const*)(base + Y * Buffer->Get_Pitch() + X) : 0;
}

unsigned char* GraphicViewPortClass::Data() { return (unsigned char*)Get_Offset(); }
unsigned char const* GraphicViewPortClass::Data() const { return (unsigned char const*)Get_Offset(); }
int GraphicViewPortClass::Pitch() const { return Get_Pitch(); }

bool GraphicViewPortClass::Lock()
{
    return Buffer ? Buffer->Lock() : false;
}

void GraphicViewPortClass::Unlock()
{
    if (Buffer) Buffer->Unlock();
}

void GraphicViewPortClass::Clear(int color)
{
    Fill_Rect(0, 0, Width - 1, Height - 1, color);
}

int GraphicViewPortClass::Get_Pixel(int x, int y) const
{
    SurfaceView view = Make_View(*this);
    return (int)Get_Pixel_View(view, x, y);
}

void GraphicViewPortClass::Put_Pixel(int x, int y, int color)
{
    SurfaceView view = Make_View(*this);
    Put_Pixel_View(view, x, y, (unsigned char)(color & 0xff));
}

void GraphicViewPortClass::Fill_Rect(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Fill_Rect_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicViewPortClass::Draw_Rect(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Draw_Rect_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicViewPortClass::Draw_Line(int x1, int y1, int x2, int y2, int color)
{
    SurfaceView view = Make_View(*this);
    Draw_Line_View(view, x1, y1, x2, y2, (unsigned char)(color & 0xff));
}

void GraphicViewPortClass::Print(char const* text, int x, int y, int fore, int back)
{
    SurfaceView view = Make_View(*this);
    Print_View(view, text, x, y, (unsigned char)(fore & 0xff), (unsigned char)(back & 0xff));
}

void GraphicViewPortClass::Blit(GraphicBufferClass& dest)
{
    Blit(dest, 0, 0);
}

void GraphicViewPortClass::Blit(GraphicBufferClass& dest, int dest_x, int dest_y)
{
    Blit(dest, 0, 0, dest_x, dest_y, Get_Width(), Get_Height(), false);
}

void GraphicViewPortClass::Blit(GraphicBufferClass& dest,
                                int src_x,
                                int src_y,
                                int dest_x,
                                int dest_y,
                                int width,
                                int height,
                                bool use_key)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Blit_View(src, dst, src_x, src_y, dest_x, dest_y, width, height, use_key);
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest)
{
    Blit(dest, 0, 0);
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest, int dest_x, int dest_y)
{
    Blit(dest, 0, 0, dest_x, dest_y, Get_Width(), Get_Height(), false);
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest,
                                int src_x,
                                int src_y,
                                int dest_x,
                                int dest_y,
                                int width,
                                int height,
                                bool use_key)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Blit_View(src, dst, src_x, src_y, dest_x, dest_y, width, height, use_key);
}

void GraphicViewPortClass::Scale(GraphicViewPortClass& dest,
                                 int src_x,
                                 int src_y,
                                 int dest_x,
                                 int dest_y,
                                 int src_w,
                                 int src_h,
                                 int dest_w,
                                 int dest_h,
                                 bool use_key,
                                 char const* remap_table)
{
    SurfaceView src = Make_View(*this);
    SurfaceView dst = Make_View(dest);
    Scale_View(src,
               dst,
               src_x,
               src_y,
               dest_x,
               dest_y,
               src_w,
               src_h,
               dest_w,
               dest_h,
               use_key,
               (unsigned char const*)remap_table);
}

int GraphicViewPortClass::To_Buffer(int x, int y, int width, int height, void* out, int out_bytes) const
{
    if (!out) return 0;
    if (width <= 0 || height <= 0) return 0;
    const int required = width * height;
    if (out_bytes < required) return 0;

    SurfaceView src = Make_View(*this);
    unsigned char* dst = (unsigned char*)out;

    for (int row = 0; row < height; ++row) {
        const int sy = y + row;
        for (int col = 0; col < width; ++col) {
            const int sx = x + col;
            dst[row * width + col] = Get_Pixel_View(src, sx, sy);
        }
    }
    return required;
}

GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page)
{
    GraphicViewPortClass* old = LogicPage;
    LogicPage = &page;
    return old;
}
