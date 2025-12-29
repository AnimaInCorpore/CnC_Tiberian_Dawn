#include "legacy/wwlib32.h"
#include "legacy/defines.h"
#include "legacy/display.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"
#include "runtime_sdl.h"

#include <SDL.h>
#include <algorithm>
#include <chrono>
#include <deque>
#include <cstring>
#include <utility>
#include <vector>

namespace {
struct GraphicBufferImpl {
  int width = 0;
  int height = 0;
  std::vector<unsigned char> storage;
};

struct ViewPortImpl {
  GraphicBufferClass* buffer = nullptr;
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  bool locked = false;
};

struct KeyboardImpl {
  std::deque<int> queue;
  bool key_down = false;
};

struct Fonts {
  const void* current = nullptr;
  const void* gradient6 = nullptr;
  int height = 0;
  int y_spacing = 0;
};

PlatformMouseState g_mouse_state{};
KeyboardImpl g_keyboard{};
Fonts g_fonts{};
SDL_Texture* g_present_texture = nullptr;
int g_present_width = 0;
int g_present_height = 0;

struct CursorState {
  const unsigned char* shape = nullptr;  // Expected to be width*height bytes of palette indices (0 treated as transparent).
  int hot_x = 0;
  int hot_y = 0;
  bool clip_enabled = false;
  int clip_left = 0;
  int clip_top = 0;
  int clip_right = 0;
  int clip_bottom = 0;
};

CursorState g_cursor{};

Uint32 Palette_Index_To_ARGB(const unsigned char* palette, int index) {
  index = std::clamp(index, 0, 255);
  const int offset = index * 3;
  const auto fetch = [palette, offset, index](int channel) -> Uint8 {
    if (!palette) {
      return static_cast<Uint8>(index);
    }
    const int value = static_cast<int>(palette[offset + channel]);
    // Palette entries are stored as VGA 6-bit values; stretch to 8-bit by
    // duplicating high bits so full-intensity white reaches 255 instead of 252.
    const int expanded = (value << 2) | (value >> 4);
    return static_cast<Uint8>(std::clamp(expanded, 0, 255));
  };
  const Uint8 r = fetch(0);
  const Uint8 g = fetch(1);
  const Uint8 b = fetch(2);
  return (0xFFu << 24) | (static_cast<Uint32>(r) << 16) | (static_cast<Uint32>(g) << 8) | b;
}

void Destroy_Present_Texture() {
  if (g_present_texture) {
    SDL_DestroyTexture(g_present_texture);
    g_present_texture = nullptr;
  }
  g_present_width = 0;
  g_present_height = 0;
}

void Ensure_Present_Texture(SDL_Renderer* renderer, int width, int height) {
  if (!renderer) return;
  if (g_present_texture && g_present_width == width && g_present_height == height) {
    return;
  }
  Destroy_Present_Texture();
  g_present_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  g_present_width = width;
  g_present_height = height;
  if (g_present_texture) {
    SDL_SetTextureScaleMode(g_present_texture, SDL_ScaleModeNearest);
  }
}

void Present_View(const GraphicViewPortClass& view) {
  SDL_Renderer* renderer = Runtime_Get_Sdl_Renderer();
  if (!renderer) return;
  const GraphicBufferClass* buffer = view.Get_Graphic_Buffer();
  if (!buffer) return;
  const unsigned char* src = buffer->Get_Buffer();
  if (!src) return;

  const int width = view.Get_Width();
  const int height = view.Get_Height();
  if (width <= 0 || height <= 0) return;

  Ensure_Present_Texture(renderer, width, height);
  if (!g_present_texture) return;

  const unsigned char* palette = Palette ? Palette : GamePalette;
  std::vector<Uint32> argb(static_cast<std::size_t>(width) * height, 0);

  const int pitch = buffer->Get_Width();
  const int origin_x = view.Get_XPos();
  const int origin_y = view.Get_YPos();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int src_index = (origin_y + y) * pitch + (origin_x + x);
      argb[static_cast<std::size_t>(y) * width + x] = Palette_Index_To_ARGB(palette, src[src_index]);
    }
  }

  SDL_UpdateTexture(g_present_texture, nullptr, argb.data(), width * sizeof(Uint32));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, g_present_texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

}  // namespace

SurfaceCollection AllSurfaces;

void SurfaceCollection::Set_Surface_Focus(bool focused) {
  if (!focused) {
    had_focus_ = false;
    return;
  }
  if (!had_focus_) {
    SurfacesRestored = true;
  }
  had_focus_ = true;
}

void SurfaceCollection::Restore_Surfaces() { SurfacesRestored = true; }

void SurfaceCollection::Release() {
  SurfacesRestored = false;
  had_focus_ = true;
}

void SurfaceCollection::Remove_DD_Surface(void* /*surface*/) {}

// --- TimerClass -----------------------------------------------------------
TimerClass::TimerClass() { Reset(0); }

TimerClass::TimerClass(long ticks) { Reset(ticks); }

void TimerClass::Reset(long ticks) {
  start_time_ = std::chrono::steady_clock::now();
  duration_ms_ = ticks;
  active_ = true;
}

void TimerClass::Clear() {
  active_ = false;
  duration_ms_ = 0;
}

long TimerClass::Time() const {
  if (!active_) {
    return 0;
  }
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
  return static_cast<long>(elapsed);
}

bool TimerClass::Expired() const {
  return active_ && duration_ms_ > 0 && Time() >= duration_ms_;
}

// --- GraphicBufferClass -----------------------------------------------------
GraphicBufferClass::GraphicBufferClass() = default;

GraphicBufferClass::GraphicBufferClass(int width, int height, void* /*data*/)
    : width_(width), height_(height), storage_(static_cast<std::size_t>(width) * height, 0) {}

void GraphicBufferClass::Init(int width, int height, void* /*data*/, int, GBC_Enum) {
  width_ = width;
  height_ = height;
  storage_.assign(static_cast<std::size_t>(width) * height, 0);
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
                              int height) const {
  dest.Blit(*this, src_x, src_y, dst_x, dst_y, width, height);
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest) const {
  dest.Blit(*this, 0, 0, 0, 0, width_, height_);
}

void GraphicBufferClass::Blit(GraphicBufferClass& dest) const {
  if (!Is_Valid() || !dest.Is_Valid()) return;
  const int copy_width = std::min(width_, dest.Get_Width());
  const int copy_height = std::min(height_, dest.Get_Height());
  const auto* src_buffer = Get_Buffer();
  auto* dst_buffer = dest.Get_Buffer();
  if (!src_buffer || !dst_buffer) return;
  for (int y = 0; y < copy_height; ++y) {
    std::memcpy(dst_buffer + y * dest.Get_Width(), src_buffer + y * width_,
                static_cast<std::size_t>(copy_width));
  }
}

void GraphicBufferClass::Blit(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
                              int height, bool) const {
  Blit(dest, src_x, src_y, dst_x, dst_y, width, height);
}

void GraphicBufferClass::Fill_Rect(int x1, int y1, int x2, int y2, int color) {
  GraphicViewPortClass view(this, 0, 0, width_, height_);
  view.Fill_Rect(x1, y1, x2, y2, color);
}

void GraphicBufferClass::Draw_Line(int x1, int y1, int x2, int y2, int color) {
  GraphicViewPortClass view(this, 0, 0, width_, height_);
  view.Draw_Line(x1, y1, x2, y2, color);
}

void GraphicBufferClass::Blit(GraphicBufferClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
                              int height) const {
  if (!Is_Valid() || !dest.Is_Valid()) return;
  const auto* src_buffer = Get_Buffer();
  auto* dst_buffer = dest.Get_Buffer();
  if (!src_buffer || !dst_buffer) return;

  const int dest_width = dest.Get_Width();
  const int dest_height = dest.Get_Height();
  const int src_width = width_;
  const int src_height = height_;

  for (int y = 0; y < height; ++y) {
    const int source_y = src_y + y;
    const int target_y = dst_y + y;
    if (source_y < 0 || target_y < 0 || source_y >= src_height || target_y >= dest_height) continue;
    for (int x = 0; x < width; ++x) {
      const int source_x = src_x + x;
      const int target_x = dst_x + x;
      if (source_x < 0 || target_x < 0 || source_x >= src_width || target_x >= dest_width) continue;
      dst_buffer[target_y * dest_width + target_x] = src_buffer[source_y * src_width + source_x];
    }
  }
}

void GraphicBufferClass::Print(char const* text, int x, int y, int fore, int back) {
  if (!text) return;
  GraphicViewPortClass view(this, 0, 0, width_, height_);
  GraphicViewPortClass* old = LogicPage;
  LogicPage = &view;
  Simple_Text_Print(text, static_cast<unsigned>(x), static_cast<unsigned>(y),
                    static_cast<unsigned>(fore), static_cast<unsigned>(back), TPF_8POINT);
  LogicPage = old;
}

void GraphicBufferClass::Print(int text_id, int x, int y, int fore, int back) {
  Print(Text_String(text_id), x, y, fore, back);
}

bool GraphicBufferClass::Is_Valid() const { return width_ > 0 && height_ > 0; }

int GraphicBufferClass::Get_Width() const { return width_; }

int GraphicBufferClass::Get_Height() const { return height_; }

bool GraphicBufferClass::Get_IsDirectDraw() const { return false; }

bool GraphicBufferClass::Lock() { return true; }

void GraphicBufferClass::Unlock() {}

void* GraphicBufferClass::Get_Offset() { return Get_Buffer(); }

const void* GraphicBufferClass::Get_Offset() const { return Get_Buffer(); }

unsigned char* GraphicBufferClass::Get_Buffer() { return storage_.empty() ? nullptr : storage_.data(); }

const unsigned char* GraphicBufferClass::Get_Buffer() const { return storage_.empty() ? nullptr : storage_.data(); }

void GraphicBufferClass::Scale(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y,
                               int width, int height, int xscale, int yscale, int, char*) {
  (void)xscale;
  (void)yscale;
  const auto* buffer = Get_Buffer();
  if (!buffer) return;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int source_x = src_x + x;
      const int source_y = src_y + y;
      if (source_x < 0 || source_y < 0 || source_x >= width_ || source_y >= height_) continue;
      const int color = buffer[source_y * width_ + source_x];
      dest.Put_Pixel(dst_x + x, dst_y + y, color);
    }
  }
}

// --- GraphicViewPortClass ---------------------------------------------------
struct GraphicViewPortClass::Impl : ViewPortImpl {};

GraphicViewPortClass::GraphicViewPortClass() : impl_(std::make_unique<Impl>()) {}

GraphicViewPortClass::GraphicViewPortClass(GraphicBufferClass* buffer, int x, int y, int width, int height)
    : impl_(std::make_unique<Impl>()) {
  Configure(buffer, x, y, width, height);
}

GraphicViewPortClass::GraphicViewPortClass(GraphicViewPortClass&&) noexcept = default;
GraphicViewPortClass& GraphicViewPortClass::operator=(GraphicViewPortClass&&) noexcept = default;
GraphicViewPortClass::~GraphicViewPortClass() = default;

void GraphicViewPortClass::Configure(GraphicBufferClass* buffer, int x, int y, int width, int height) {
  impl_->buffer = buffer;
  impl_->x = x;
  impl_->y = y;
  impl_->width = width;
  impl_->height = height;
}

bool GraphicViewPortClass::Lock() {
  impl_->locked = true;
  return true;
}

void GraphicViewPortClass::Unlock() { impl_->locked = false; }

void GraphicViewPortClass::Clear() {
  auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return;
  for (int y = impl_->y; y < impl_->y + impl_->height; ++y) {
    for (int x = impl_->x; x < impl_->x + impl_->width; ++x) {
      Put_Pixel(x, y, 0);
    }
  }
}

void GraphicViewPortClass::Fill_Rect(int x1, int y1, int x2, int y2, int color) {
  auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return;

  const int left = std::min(x1, x2);
  const int right = std::max(x1, x2);
  const int top = std::min(y1, y2);
  const int bottom = std::max(y1, y2);

  for (int y = top; y <= bottom; ++y) {
    const int rel_y = y - impl_->y;
    if (rel_y < 0 || rel_y >= impl_->height) continue;
    const int row_offset = rel_y * impl_->buffer->Get_Width();
    for (int x = left; x <= right; ++x) {
      const int rel_x = x - impl_->x;
      if (rel_x < 0 || rel_x >= impl_->width) continue;
      buffer[row_offset + rel_x] = static_cast<unsigned char>(color);
    }
  }
}

void GraphicViewPortClass::Draw_Line(int x1, int y1, int x2, int y2, int color) {
  int dx = std::abs(x2 - x1);
  int dy = -std::abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx + dy;

  int x = x1;
  int y = y1;
  while (true) {
    Put_Pixel(x, y, color);
    if (x == x2 && y == y2) break;
    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y += sy;
    }
  }
}

void GraphicViewPortClass::Draw_Rect(int x1, int y1, int x2, int y2, int color) {
  Draw_Line(x1, y1, x2, y1, color);
  Draw_Line(x1, y2, x2, y2, color);
  Draw_Line(x1, y1, x1, y2, color);
  Draw_Line(x2, y1, x2, y2, color);
}

void GraphicViewPortClass::Draw_Stamp(void const* iconset, int icon, int x, int y, void const*, int) {
  if (!iconset || !impl_->buffer) return;
  const auto* base = static_cast<const unsigned char*>(iconset);

  long map_offset = 0;
  std::memcpy(&map_offset, base + 28, sizeof(map_offset));
  const unsigned char* map = map_offset ? base + map_offset : nullptr;

  int actual_icon = icon;
  if (map) {
    actual_icon = static_cast<int>(map[icon]);
    if (actual_icon == 0xFF) return;
  }

  long data_offset = 0;
  std::memcpy(&data_offset, base + 12, sizeof(data_offset));
  if (data_offset <= 0) return;

  const unsigned char* data =
      base + data_offset + static_cast<long>(actual_icon) * (ICON_PIXEL_W * ICON_PIXEL_H);
  for (int row = 0; row < ICON_PIXEL_H; ++row) {
    for (int col = 0; col < ICON_PIXEL_W; ++col) {
      const int color = data[row * ICON_PIXEL_W + col];
      Put_Pixel(x + col, y + row, color);
    }
  }
}

void GraphicViewPortClass::Remap(int x, int y, int width, int height, const unsigned char* table) {
  auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer || !table) return;

  const int left = x;
  const int right = x + width - 1;
  const int top = y;
  const int bottom = y + height - 1;

  for (int yy = top; yy <= bottom; ++yy) {
    const int rel_y = yy - impl_->y;
    if (rel_y < 0 || rel_y >= impl_->height) continue;
    const int row_offset = rel_y * impl_->buffer->Get_Width();
    for (int xx = left; xx <= right; ++xx) {
      const int rel_x = xx - impl_->x;
      if (rel_x < 0 || rel_x >= impl_->width) continue;
      unsigned char& pixel = buffer[row_offset + rel_x];
      pixel = table[pixel];
    }
  }
}

void GraphicViewPortClass::Put_Pixel(int x, int y, int color) {
  auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return;
  if (x < impl_->x || y < impl_->y) return;
  const int rel_x = x - impl_->x;
  const int rel_y = y - impl_->y;
  if (rel_x < 0 || rel_y < 0 || rel_x >= impl_->width || rel_y >= impl_->height) return;
  buffer[rel_y * impl_->buffer->Get_Width() + rel_x] = static_cast<unsigned char>(color);
}

int GraphicViewPortClass::Get_Pixel(int x, int y) const {
  const auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return 0;
  if (x < impl_->x || y < impl_->y) return 0;
  const int rel_x = x - impl_->x;
  const int rel_y = y - impl_->y;
  if (rel_x < 0 || rel_y < 0 || rel_x >= impl_->width || rel_y >= impl_->height) return 0;
  return buffer[rel_y * impl_->buffer->Get_Width() + rel_x];
}

void GraphicViewPortClass::To_Buffer(int x, int y, int width, int height, void* dest, int dest_size) const {
  if (!dest || width <= 0 || height <= 0 || dest_size <= 0) return;
  const auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return;

  const int start_x = std::max(x, impl_->x);
  const int start_y = std::max(y, impl_->y);
  const int end_x = std::min(x + width, impl_->x + impl_->width);
  const int end_y = std::min(y + height, impl_->y + impl_->height);
  if (start_x >= end_x || start_y >= end_y) return;

  const int copy_width = end_x - start_x;
  const int copy_height = end_y - start_y;
  if (dest_size < copy_width * copy_height) return;

  const int buffer_width = impl_->buffer->Get_Width();
  auto* out = static_cast<unsigned char*>(dest);
  const int dest_x = start_x - x;
  const int dest_y = start_y - y;

  for (int row = 0; row < copy_height; ++row) {
    const int src_row = (start_y - impl_->y) + row;
    const int dst_row = dest_y + row;
    const unsigned char* src_row_ptr = buffer + src_row * buffer_width + (start_x - impl_->x);
    unsigned char* dst_row_ptr = out + dst_row * width + dest_x;
    std::memcpy(dst_row_ptr, src_row_ptr, static_cast<std::size_t>(copy_width));
  }
}

bool GraphicViewPortClass::Contains(int x, int y) const {
  return x >= impl_->x && y >= impl_->y && x < impl_->x + impl_->width && y < impl_->y + impl_->height;
}

int GraphicViewPortClass::Get_XPos() const { return impl_->x; }
int GraphicViewPortClass::Get_YPos() const { return impl_->y; }
int GraphicViewPortClass::Get_Width() const { return impl_->width; }
int GraphicViewPortClass::Get_Height() const { return impl_->height; }
int GraphicViewPortClass::Get_XAdd() const { return 0; }
int GraphicViewPortClass::Get_Pitch() const {
  return impl_->buffer ? (impl_->buffer->Get_Width() - impl_->width) : 0;
}
void* GraphicViewPortClass::Get_Offset() {
  if (!impl_->buffer) return nullptr;
  auto* buffer = impl_->buffer->Get_Buffer();
  if (!buffer) return nullptr;
  return buffer + impl_->y * impl_->buffer->Get_Width() + impl_->x;
}
const void* GraphicViewPortClass::Get_Offset() const {
  if (!impl_->buffer) return nullptr;
  const auto* buffer = impl_->buffer->Get_Buffer();
  if (!buffer) return nullptr;
  return buffer + impl_->y * impl_->buffer->Get_Width() + impl_->x;
}
GraphicBufferClass* GraphicViewPortClass::Get_Graphic_Buffer() const { return impl_->buffer; }
bool GraphicViewPortClass::Get_IsDirectDraw() const { return false; }

void GraphicViewPortClass::Blit(const GraphicBufferClass& src, int src_x, int src_y, int dst_x, int dst_y,
                                int width, int height) {
  auto* dest_buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  const auto* src_buffer = src.Get_Buffer();
  if (!dest_buffer || !src_buffer) return;

  for (int y = 0; y < height; ++y) {
    const int source_y = src_y + y;
    const int dest_y = dst_y + y - impl_->y;
    if (source_y < 0 || dest_y < 0 || source_y >= src.Get_Height() || dest_y >= impl_->height) continue;
    for (int x = 0; x < width; ++x) {
      const int source_x = src_x + x;
      const int dest_x = dst_x + x - impl_->x;
      if (source_x < 0 || dest_x < 0 || source_x >= src.Get_Width() || dest_x >= impl_->width) continue;
      dest_buffer[dest_y * impl_->buffer->Get_Width() + dest_x] = src_buffer[source_y * src.Get_Width() + source_x];
    }
  }

  if (this == &SeenBuff) {
    Present_View(*this);
  }
}

void GraphicViewPortClass::Blit(const GraphicBufferClass& src) {
  Blit(src, 0, 0, impl_->x, impl_->y, src.Get_Width(), src.Get_Height());
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
                                int height) const {
  const auto* src_storage = impl_->buffer;
  auto* dest_storage = dest.impl_->buffer;
  if (!src_storage || !dest_storage) return;
  const auto* src_buffer = src_storage->Get_Buffer();
  auto* dest_buffer = dest_storage->Get_Buffer();
  if (!src_buffer || !dest_buffer) return;

  for (int y = 0; y < height; ++y) {
    const int source_y = src_y + y - impl_->y;
    const int dest_y = dst_y + y - dest.impl_->y;
    if (source_y < 0 || dest_y < 0 || source_y >= src_storage->Get_Height() || dest_y >= dest.impl_->height) continue;
    for (int x = 0; x < width; ++x) {
      const int source_x = src_x + x - impl_->x;
      const int dest_x = dst_x + x - dest.impl_->x;
      if (source_x < 0 || dest_x < 0 || source_x >= src_storage->Get_Width() || dest_x >= dest.impl_->width) continue;
      dest_buffer[dest_y * dest_storage->Get_Width() + dest_x] =
          src_buffer[source_y * src_storage->Get_Width() + source_x];
    }
  }

  if (&dest == &SeenBuff) {
    Present_View(dest);
  }
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest, int dst_x, int dst_y) const {
  Blit(dest, impl_->x, impl_->y, dst_x, dst_y, impl_->width, impl_->height);
}

void GraphicViewPortClass::Blit(GraphicViewPortClass& dest) const {
  Blit(dest, impl_->x, impl_->y, dest.impl_->x, dest.impl_->y, impl_->width, impl_->height);
}

void GraphicViewPortClass::Scale(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
                                 int height, int, int, char*) {
  auto* buffer = impl_->buffer ? impl_->buffer->Get_Buffer() : nullptr;
  if (!buffer) return;
  const int buffer_width = impl_->buffer->Get_Width();
  const int buffer_height = impl_->buffer->Get_Height();

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int src_abs_x = impl_->x + src_x + x;
      const int src_abs_y = impl_->y + src_y + y;
      if (src_abs_x < 0 || src_abs_y < 0 || src_abs_x >= buffer_width || src_abs_y >= buffer_height) {
        continue;
      }
      const int color = buffer[src_abs_y * buffer_width + src_abs_x];
      dest.Put_Pixel(dst_x + x, dst_y + y, color);
    }
  }
}

// --- WWMouseClass -----------------------------------------------------------
struct WWMouseClass::Impl {
  int width = 0;
  int height = 0;
  bool is_drawn = false;
  int last_x = 0;
  int last_y = 0;
  std::vector<unsigned char> saved;
};

WWMouseClass::WWMouseClass() : impl_(std::make_unique<Impl>()) {}
WWMouseClass::WWMouseClass(GraphicViewPortClass* /*page*/, int width, int height)
    : impl_(std::make_unique<Impl>()) {
  impl_->width = std::max(1, width);
  impl_->height = std::max(1, height);
  impl_->saved.assign(static_cast<std::size_t>(impl_->width) * impl_->height, 0);
}
WWMouseClass::WWMouseClass(WWMouseClass&&) noexcept = default;
WWMouseClass& WWMouseClass::operator=(WWMouseClass&&) noexcept = default;
WWMouseClass::~WWMouseClass() = default;

void WWMouseClass::Draw_Mouse(GraphicViewPortClass* page) {
  if (!page || !g_cursor.shape) return;

  const int mouse_x = Get_Mouse_X();
  const int mouse_y = Get_Mouse_Y();
  const int draw_x = mouse_x - g_cursor.hot_x;
  const int draw_y = mouse_y - g_cursor.hot_y;

  // If we are already drawn at this position, don't do work twice.
  if (impl_->is_drawn && impl_->last_x == draw_x && impl_->last_y == draw_y) {
    return;
  }

  // If the cursor is drawn elsewhere, erase it first.
  if (impl_->is_drawn) {
    Erase_Mouse(page, true);
  }

  impl_->last_x = draw_x;
  impl_->last_y = draw_y;
  impl_->is_drawn = true;

  // Save background, then draw cursor.
  for (int y = 0; y < impl_->height; ++y) {
    for (int x = 0; x < impl_->width; ++x) {
      const int px = draw_x + x;
      const int py = draw_y + y;
      const std::size_t idx = static_cast<std::size_t>(y) * impl_->width + x;
      impl_->saved[idx] = static_cast<unsigned char>(page->Get_Pixel(px, py));
    }
  }

  for (int y = 0; y < impl_->height; ++y) {
    for (int x = 0; x < impl_->width; ++x) {
      const int px = draw_x + x;
      const int py = draw_y + y;
      if (g_cursor.clip_enabled) {
        if (px < g_cursor.clip_left || px > g_cursor.clip_right || py < g_cursor.clip_top || py > g_cursor.clip_bottom) {
          continue;
        }
      }
      const unsigned char color = g_cursor.shape[static_cast<std::size_t>(y) * impl_->width + x];
      if (color != 0) {
        page->Put_Pixel(px, py, color);
      }
    }
  }
}

void WWMouseClass::Erase_Mouse(GraphicViewPortClass* page, bool /*force*/) {
  if (!page || !impl_->is_drawn) return;
  for (int y = 0; y < impl_->height; ++y) {
    for (int x = 0; x < impl_->width; ++x) {
      const int px = impl_->last_x + x;
      const int py = impl_->last_y + y;
      const std::size_t idx = static_cast<std::size_t>(y) * impl_->width + x;
      page->Put_Pixel(px, py, impl_->saved[idx]);
    }
  }
  impl_->is_drawn = false;
}

void WWMouseClass::Clear_Cursor_Clip() { g_cursor.clip_enabled = false; }
void WWMouseClass::Set_Cursor_Clip() {
  // Mirror the legacy behaviour: clip the cursor to the active screen bounds.
  // The game UI assumes the cursor cannot leave the drawable region.
  g_cursor.clip_enabled = true;
  g_cursor.clip_left = 0;
  g_cursor.clip_top = 0;
  g_cursor.clip_right = std::max(0, ScreenWidth - 1);
  g_cursor.clip_bottom = std::max(0, ScreenHeight - 1);
}

// --- WWKeyboardClass --------------------------------------------------------
struct WWKeyboardClass::Impl : KeyboardImpl {};

WWKeyboardClass::WWKeyboardClass() : impl_(std::make_unique<Impl>()) {}
WWKeyboardClass::WWKeyboardClass(WWKeyboardClass&&) noexcept = default;
WWKeyboardClass& WWKeyboardClass::operator=(WWKeyboardClass&&) noexcept = default;
WWKeyboardClass::~WWKeyboardClass() = default;

int WWKeyboardClass::Get() {
  if (impl_->queue.empty()) return KN_NONE;
  const int key = impl_->queue.front();
  impl_->queue.pop_front();
  return key;
}

int WWKeyboardClass::Check() const { return impl_->queue.empty() ? KN_NONE : impl_->queue.front(); }

void WWKeyboardClass::Clear() { impl_->queue.clear(); }

void WWKeyboardClass::Stuff(int key) { impl_->queue.push_back(key); }

bool WWKeyboardClass::Down(int key) const { return impl_->key_down && !impl_->queue.empty() && impl_->queue.front() == key; }

void WWKeyboardClass::Message_Handler(HWND /*hwnd*/, unsigned int /*message*/, WPARAM wparam, LPARAM /*lparam*/) {
  impl_->queue.push_back(static_cast<int>(wparam));
  impl_->key_down = true;
}

// --- Global helpers ---------------------------------------------------------
GraphicViewPortClass* LogicPage = nullptr;

GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page) {
  GraphicViewPortClass* previous = LogicPage;
  LogicPage = &page;
  return previous;
}

GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass* page) {
  GraphicViewPortClass* previous = LogicPage;
  LogicPage = page;
  return previous;
}

GraphicViewPortClass* Set_Logic_Page(GraphicBufferClass& page) {
  static GraphicViewPortClass buffer_view;
  buffer_view.Configure(&page, 0, 0, page.Get_Width(), page.Get_Height());
  return Set_Logic_Page(buffer_view);
}

GraphicViewPortClass* Set_Logic_Page(GraphicBufferClass* page) {
  if (!page) {
    return Set_Logic_Page(static_cast<GraphicViewPortClass*>(nullptr));
  }
  return Set_Logic_Page(*page);
}

namespace {
void Apply_Sdl_Video_Mode(int width, int height) {
  if (width <= 0 || height <= 0) return;
  SDL_Window* window = Runtime_Get_Sdl_Window();
  SDL_Renderer* renderer = Runtime_Get_Sdl_Renderer();
  if (!window || !renderer) return;

  SDL_SetWindowSize(window, width, height);
  SDL_RenderSetLogicalSize(renderer, width, height);
  SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
  Destroy_Present_Texture();
}
}  // namespace

bool Set_Video_Mode(int mode) {
  // The legacy runtime used a mode integer; the port maps this onto the
  // current screen size defaults and reconfigures SDL's logical output.
  if (mode == RESET_MODE) {
    Apply_Sdl_Video_Mode(ScreenWidth, ScreenHeight);
    return true;
  }
  if (mode == MCGA_MODE) {
    ScreenWidth = DEFAULT_SCREEN_WIDTH;
    ScreenHeight = 400;
    Apply_Sdl_Video_Mode(ScreenWidth, ScreenHeight);
    return true;
  }
  // Unknown modes are treated as "keep current".
  Apply_Sdl_Video_Mode(ScreenWidth, ScreenHeight);
  return true;
}

bool Set_Video_Mode(void* /*window*/, int width, int height, int /*bits_per_pixel*/) {
  if (width <= 0 || height <= 0) return false;
  ScreenWidth = width;
  ScreenHeight = height;
  Apply_Sdl_Video_Mode(ScreenWidth, ScreenHeight);
  return true;
}

int Get_Key_Num() { return Kbd.Get(); }
int Check_Key_Num() { return Kbd.Check(); }
int KN_To_KA(int key) { return key; }
void Clear_KeyBuffer() { Kbd.Clear(); }
void Stuff_Key_Num(int key) { Kbd.Stuff(key); }
int Key_Down(int key) { return Kbd.Down(key) ? 1 : 0; }
int Check_Key() { return Check_Key_Num(); }
int Get_Key() { return Get_Key_Num(); }

int Get_Mouse_X() { return g_mouse_state.x; }
int Get_Mouse_Y() { return g_mouse_state.y; }

void Update_Mouse_Position(int x, int y) {
  if (g_cursor.clip_enabled) {
    x = std::clamp(x, g_cursor.clip_left, g_cursor.clip_right);
    y = std::clamp(y, g_cursor.clip_top, g_cursor.clip_bottom);
  }
  g_mouse_state.x = x;
  g_mouse_state.y = y;
}

void Set_Mouse_Cursor(int hotx, int hoty, void const* shape) {
  g_cursor.hot_x = hotx;
  g_cursor.hot_y = hoty;
  g_cursor.shape = static_cast<const unsigned char*>(shape);
}

void Set_Font_Palette_Range(void const* palette, int first, int count) {
  // Legacy `Set_Font_Palette[_Range]` updates the ColorXlat translation table
  // (not the VGA palette). The macro in COMPAT.H calls this with (0, 15) to
  // update all 16 font indices.
  if (!palette) return;

  auto* xlat = const_cast<unsigned char*>(
      static_cast<const unsigned char*>(Get_Font_Palette_Ptr()));
  if (!xlat) return;

  const auto* source = static_cast<const unsigned char*>(palette);
  const int start = std::clamp(first, 0, 15);
  const int end = std::clamp(count, start, 15);

  for (int idx = start; idx <= end; ++idx) {
    xlat[idx] = source[idx];
    // Mirror into the high-nibble indices (0x10, 0x20, ...) used by the ASM
    // text renderer's xlat lookup path.
    xlat[idx << 4] = source[idx];
  }
}

extern "C" void ModeX_Blit(GraphicBufferClass* source) {
  (void)source;
  GScreenClass::Blit_Display();
}

void const* Set_Current_Font(void const* font) {
  const void* old = g_fonts.current;
  g_fonts.current = font;
  return old;
}

void const* Get_Current_Font() { return g_fonts.current; }

void Set_Gradient_Font_6(void const* font) { g_fonts.gradient6 = font; }

void const* Get_Gradient_Font_6() { return g_fonts.gradient6; }

void Platform_Set_Fonts(const void* current_font, const void* gradient_font6, int font_height, int font_y_spacing) {
  g_fonts.current = current_font;
  g_fonts.gradient6 = gradient_font6;
  g_fonts.height = font_height;
  g_fonts.y_spacing = font_y_spacing;
}

void Platform_Update_Mouse_State(const PlatformMouseState& state) {
  g_mouse_state = state;
  Update_Mouse_Position(state.x, state.y);
  Kbd.MouseQX = g_mouse_state.x;
  Kbd.MouseQY = g_mouse_state.y;
}

void Platform_Queue_Key_Event(int key, bool pressed) {
  const int queued_key = pressed ? key : (key | KN_RLSE_BIT);
  Kbd.Stuff(queued_key);
  Kbd.impl_->key_down = pressed;
}
