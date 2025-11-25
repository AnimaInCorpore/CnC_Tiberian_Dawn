#include "legacy/wwlib32.h"
#include "legacy/defines.h"
#include "legacy/externs.h"

#include <algorithm>
#include <deque>
#include <cstring>
#include <utility>

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

}  // namespace

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

bool GraphicBufferClass::Is_Valid() const { return width_ > 0 && height_ > 0; }

int GraphicBufferClass::Get_Width() const { return width_; }

int GraphicBufferClass::Get_Height() const { return height_; }

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
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  (void)color;
}

void GraphicViewPortClass::Draw_Line(int x1, int y1, int x2, int y2, int color) {
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  (void)color;
}

void GraphicViewPortClass::Draw_Rect(int x1, int y1, int x2, int y2, int color) {
  Fill_Rect(x1, y1, x2, y2, color);
}

void GraphicViewPortClass::Remap(int x, int y, int width, int height, const unsigned char* table) {
  (void)table;
  Fill_Rect(x, y, x + width - 1, y + height - 1, 0);
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

bool GraphicViewPortClass::Contains(int x, int y) const {
  return x >= impl_->x && y >= impl_->y && x < impl_->x + impl_->width && y < impl_->y + impl_->height;
}

int GraphicViewPortClass::Get_XPos() const { return impl_->x; }
int GraphicViewPortClass::Get_YPos() const { return impl_->y; }
int GraphicViewPortClass::Get_Width() const { return impl_->width; }
int GraphicViewPortClass::Get_Height() const { return impl_->height; }
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
}

void GraphicViewPortClass::Blit(const GraphicViewPortClass& src, int src_x, int src_y, int dst_x, int dst_y,
                                int width, int height) {
  const auto* src_buffer = src.Get_Graphic_Buffer() ? src.Get_Graphic_Buffer()->Get_Buffer() : nullptr;
  if (!src_buffer) return;
  if (!impl_->buffer) return;
  GraphicBufferClass* src_storage = src.Get_Graphic_Buffer();
  for (int y = 0; y < height; ++y) {
    const int source_y = src_y + y - src.Get_YPos();
    const int dest_y = dst_y + y - impl_->y;
    if (source_y < 0 || dest_y < 0 || source_y >= src_storage->Get_Height() || dest_y >= impl_->height) continue;
    for (int x = 0; x < width; ++x) {
      const int source_x = src_x + x - src.Get_XPos();
      const int dest_x = dst_x + x - impl_->x;
      if (source_x < 0 || dest_x < 0 || source_x >= src_storage->Get_Width() || dest_x >= impl_->width) continue;
      impl_->buffer
          ->Get_Buffer()[dest_y * impl_->buffer->Get_Width() + dest_x] =
          src_buffer[source_y * src_storage->Get_Width() + source_x];
    }
  }
}

void GraphicViewPortClass::Blit(const GraphicViewPortClass& src, int dst_x, int dst_y) {
  Blit(src, src.Get_XPos(), src.Get_YPos(), dst_x, dst_y, src.Get_Width(), src.Get_Height());
}

void GraphicViewPortClass::Blit(const GraphicViewPortClass& src) {
  Blit(src, src.Get_XPos(), src.Get_YPos(), src.Get_XPos(), src.Get_YPos(), src.Get_Width(), src.Get_Height());
}

// --- WWMouseClass -----------------------------------------------------------
struct WWMouseClass::Impl {};

WWMouseClass::WWMouseClass() : impl_(std::make_unique<Impl>()) {}
WWMouseClass::WWMouseClass(GraphicViewPortClass* /*page*/, int /*width*/, int /*height*/)
    : impl_(std::make_unique<Impl>()) {}
WWMouseClass::WWMouseClass(WWMouseClass&&) noexcept = default;
WWMouseClass& WWMouseClass::operator=(WWMouseClass&&) noexcept = default;
WWMouseClass::~WWMouseClass() = default;

void WWMouseClass::Draw_Mouse(GraphicViewPortClass* /*page*/) {}
void WWMouseClass::Erase_Mouse(GraphicViewPortClass* /*page*/, bool /*force*/) {}
void WWMouseClass::Clear_Cursor_Clip() {}
void WWMouseClass::Set_Cursor_Clip() {}

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

bool Set_Video_Mode(int /*mode*/) { return true; }

bool Set_Video_Mode(void* /*window*/, int /*width*/, int /*height*/, int /*bits_per_pixel*/) { return true; }

static WWKeyboardClass g_keyboard_instance;

int Get_Key_Num() { return g_keyboard_instance.Get(); }
int Check_Key_Num() { return g_keyboard_instance.Check(); }
int KN_To_KA(int key) { return key; }
void Clear_KeyBuffer() { g_keyboard_instance.Clear(); }
void Stuff_Key_Num(int key) { g_keyboard_instance.Stuff(key); }
int Key_Down(int key) { return g_keyboard_instance.Down(key) ? 1 : 0; }

int Get_Mouse_X() { return g_mouse_state.x; }
int Get_Mouse_Y() { return g_mouse_state.y; }

void Update_Mouse_Position(int x, int y) {
  g_mouse_state.x = x;
  g_mouse_state.y = y;
}

void Set_Font_Palette_Range(void const* palette, int first, int count) {
  if (!palette || count <= 0) return;
  if (!GamePalette && !Palette) return;
  if (!GamePalette) {
    GamePalette = new unsigned char[256 * 3];
    std::fill_n(GamePalette, 256 * 3, 0);
  }
  if (!Palette) {
    Palette = new unsigned char[256 * 3];
    std::fill_n(Palette, 256 * 3, 0);
  }
  const auto* source = static_cast<const unsigned char*>(palette);
  const int start = std::clamp(first, 0, 255);
  const int clamped_count = std::max(0, std::min(count, 256 - start));
  const int byte_offset = start * 3;
  const int byte_count = clamped_count * 3;

  if (GamePalette) {
    std::memcpy(GamePalette + byte_offset, source + byte_offset, byte_count);
  }
  if (Palette) {
    std::memcpy(Palette + byte_offset, source + byte_offset, byte_count);
  }
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

void Platform_Update_Mouse_State(const PlatformMouseState& state) { g_mouse_state = state; }

void Platform_Queue_Key_Event(int key, bool pressed) {
  if (pressed) {
    g_keyboard_instance.Stuff(key);
  }
  g_keyboard_instance.impl_->key_down = pressed;
}
