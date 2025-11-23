#include "legacy/wwlib32.h"

#include <algorithm>
#include <deque>
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

bool GraphicBufferClass::Is_Valid() const { return width_ > 0 && height_ > 0; }

int GraphicBufferClass::Get_Width() const { return width_; }

int GraphicBufferClass::Get_Height() const { return height_; }

unsigned char* GraphicBufferClass::Get_Buffer() { return storage_.empty() ? nullptr : storage_.data(); }

const unsigned char* GraphicBufferClass::Get_Buffer() const { return storage_.empty() ? nullptr : storage_.data(); }

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
  LogicPage = &page;
  return LogicPage;
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

void Set_Font_Palette_Range(void const* /*palette*/, int /*first*/, int /*count*/) {}

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
