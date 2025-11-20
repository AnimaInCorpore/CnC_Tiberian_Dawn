#include "wwlib32.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_set>

extern "C" {
int MouseQX = 0;
int MouseQY = 0;
}

namespace {
constexpr double kTau = 6.28318530717958647692;
constexpr char kEmptyString[] = "";

constexpr int kDefaultScreenWidth = 640;
constexpr int kDefaultScreenHeight = 480;
constexpr int kMcgaWidth = 320;
constexpr int kMcgaHeight = 200;
constexpr std::uint8_t kMouseOutlineColor = 0xF0;
constexpr std::uint8_t kMouseCenterColor = 0xFF;
constexpr std::size_t kKeyQueueSize = 64;

int Normalize_Key(int key) {
  return key & ~KN_RLSE_BIT;
}

std::uint8_t NormalizeAscii(int key) {
  return static_cast<std::uint8_t>(Normalize_Key(key) & 0xFF);
}

struct KeyQueue {
  void Push(int key) {
    buffer_[tail_] = key;
    tail_ = (tail_ + 1) % buffer_.size();
    if (count_ == buffer_.size()) {
      head_ = (head_ + 1) % buffer_.size();
    } else {
      ++count_;
    }
  }

  int Pop() {
    if (count_ == 0) {
      return KN_NONE;
    }
    const int value = buffer_[head_];
    head_ = (head_ + 1) % buffer_.size();
    --count_;
    return value;
  }

  int Peek() const {
    if (count_ == 0) {
      return KN_NONE;
    }
    return buffer_[head_];
  }

  void Clear() {
    head_ = 0;
    tail_ = 0;
    count_ = 0;
  }

 private:
  std::array<int, kKeyQueueSize> buffer_{};
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t count_ = 0;
};

void Sync_Mouse_Event_Position(WWKeyboardClass& keyboard, int x, int y) {
  keyboard.MouseQX = x;
  keyboard.MouseQY = y;
  MouseQX = x;
  MouseQY = y;
}

template <typename T>
T ClampValue(T value, T low, T high) {
  return std::max(low, std::min(value, high));
}

struct MousePosition {
  int x = 0;
  int y = 0;
};

struct MouseClip {
  bool enabled = false;
  int left = 0;
  int top = 0;
  int right = std::numeric_limits<int>::max();
  int bottom = std::numeric_limits<int>::max();
};

MousePosition g_mouse_position;
MouseClip g_mouse_clip;
void const* g_current_font = nullptr;
void const* g_grad_font6 = nullptr;
}  // namespace

class GraphicBufferClass {
 public:
  GraphicBufferClass();
  GraphicBufferClass(int width, int height, void* data = nullptr);

  void Resize(int width, int height);
  int Get_Width() const { return width_; }
  int Get_Height() const { return height_; }
  std::uint8_t Get_Pixel(int x, int y) const;
  void Put_Pixel(int x, int y, std::uint8_t color);

 private:
  int width_ = 0;
  int height_ = 0;
  std::vector<std::uint8_t> pixels_;
};

GraphicBufferClass::GraphicBufferClass() = default;

GraphicBufferClass::GraphicBufferClass(int width, int height, void*)
    : width_(0), height_(0) {
  Resize(width, height);
}

void GraphicBufferClass::Resize(int width, int height) {
  width_ = std::max(width, 0);
  height_ = std::max(height, 0);

  const std::size_t size =
      static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
  pixels_.assign(size, 0);
}

std::uint8_t GraphicBufferClass::Get_Pixel(int x, int y) const {
  if (x < 0 || y < 0 || x >= width_ || y >= height_) {
    return 0;
  }
  const std::size_t offset =
      static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
      static_cast<std::size_t>(x);
  return pixels_[offset];
}

void GraphicBufferClass::Put_Pixel(int x, int y, std::uint8_t color) {
  if (x < 0 || y < 0 || x >= width_ || y >= height_) {
    return;
  }
  const std::size_t offset =
      static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
      static_cast<std::size_t>(x);
  pixels_[offset] = color;
}

GraphicBufferClass VisiblePage(kDefaultScreenWidth, kDefaultScreenHeight);
GraphicBufferClass HiddenPage(kDefaultScreenWidth, kDefaultScreenHeight);
GraphicBufferClass ModeXBuff(kDefaultScreenWidth, kDefaultScreenHeight);
GraphicBufferClass LoResHidPage(kDefaultScreenWidth, kDefaultScreenHeight);
GraphicBufferClass SysMemPage(kDefaultScreenWidth, kDefaultScreenHeight);

GraphicViewPortClass SeenBuff(&VisiblePage, 0, 0, kDefaultScreenWidth,
                              kDefaultScreenHeight);
GraphicViewPortClass HidPage(&HiddenPage, 0, 0, kDefaultScreenWidth,
                             kDefaultScreenHeight);

GraphicViewPortClass* LogicPage = &SeenBuff;
WWMouseClass* WWMouse = nullptr;

namespace {
std::uint8_t Normalize_Color(int color) {
  return static_cast<std::uint8_t>(ClampValue(color, 0, 255));
}

void Resize_Framebuffers(int width, int height) {
  width = std::max(width, 1);
  height = std::max(height, 1);

  VisiblePage.Resize(width, height);
  HiddenPage.Resize(width, height);
  ModeXBuff.Resize(width, height);
  LoResHidPage.Resize(width, height);
  SysMemPage.Resize(width, height);

  SeenBuff.Configure(&VisiblePage, 0, 0, width, height);
  HidPage.Configure(&HiddenPage, 0, 0, width, height);
}

struct CursorPixel {
  int x;
  int y;
  std::uint8_t color;
};
}  // namespace

struct GraphicViewPortClass::Impl {
  GraphicBufferClass* buffer = nullptr;
  int left = 0;
  int top = 0;
  int width = 0;
  int height = 0;
  int lock_depth = 0;

  void Configure(GraphicBufferClass* new_buffer, int x, int y, int w, int h) {
    buffer = new_buffer;
    left = x;
    top = y;
    width = std::max(w, 0);
    height = std::max(h, 0);
  }

  bool Contains(int x, int y) const {
    if (!buffer || width <= 0 || height <= 0) {
      return false;
    }
    if (x < left || x > left + width - 1) {
      return false;
    }
    if (y < top || y > top + height - 1) {
      return false;
    }
    if (x < 0 || x >= buffer->Get_Width()) {
      return false;
    }
    if (y < 0 || y >= buffer->Get_Height()) {
      return false;
    }
    return true;
  }

  bool ClipRect(int& x1, int& y1, int& x2, int& y2) const {
    if (!buffer || width <= 0 || height <= 0) {
      return false;
    }
    if (x1 > x2) {
      std::swap(x1, x2);
    }
    if (y1 > y2) {
      std::swap(y1, y2);
    }

    const int view_right = left + width - 1;
    const int view_bottom = top + height - 1;
    const int buffer_right = buffer->Get_Width() - 1;
    const int buffer_bottom = buffer->Get_Height() - 1;

    x1 = std::max(x1, std::max(left, 0));
    y1 = std::max(y1, std::max(top, 0));
    x2 = std::min(x2, std::min(view_right, buffer_right));
    y2 = std::min(y2, std::min(view_bottom, buffer_bottom));
    return x1 <= x2 && y1 <= y2;
  }
};

GraphicViewPortClass::GraphicViewPortClass()
    : impl_(std::make_unique<Impl>()) {}

GraphicViewPortClass::GraphicViewPortClass(GraphicBufferClass* buffer, int x,
                                           int y, int width, int height)
    : GraphicViewPortClass() {
  Configure(buffer, x, y, width, height);
}

GraphicViewPortClass::GraphicViewPortClass(GraphicViewPortClass&&) noexcept =
    default;
GraphicViewPortClass& GraphicViewPortClass::operator=(
    GraphicViewPortClass&&) noexcept = default;

GraphicViewPortClass::~GraphicViewPortClass() = default;

void GraphicViewPortClass::Configure(GraphicBufferClass* buffer, int x, int y,
                                     int width, int height) {
  impl_->Configure(buffer, x, y, width, height);
}

bool GraphicViewPortClass::Lock() {
  if (!impl_->buffer) {
    return false;
  }
  ++impl_->lock_depth;
  return true;
}

void GraphicViewPortClass::Unlock() {
  if (impl_->lock_depth > 0) {
    --impl_->lock_depth;
  }
}

void GraphicViewPortClass::Fill_Rect(int x1, int y1, int x2, int y2, int color) {
  if (!impl_->buffer) {
    return;
  }
  if (!impl_->ClipRect(x1, y1, x2, y2)) {
    return;
  }
  const std::uint8_t fill = Normalize_Color(color);
  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      impl_->buffer->Put_Pixel(x, y, fill);
    }
  }
}

void GraphicViewPortClass::Draw_Line(int x1, int y1, int x2, int y2, int color) {
  if (!impl_->buffer) {
    return;
  }
  const std::uint8_t draw_color = Normalize_Color(color);
  int dx = std::abs(x2 - x1);
  int sx = (x1 < x2) ? 1 : -1;
  int dy = -std::abs(y2 - y1);
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx + dy;

  while (true) {
    Put_Pixel(x1, y1, draw_color);
    if (x1 == x2 && y1 == y2) {
      break;
    }
    const int err2 = 2 * err;
    if (err2 >= dy) {
      err += dy;
      x1 += sx;
    }
    if (err2 <= dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void GraphicViewPortClass::Draw_Rect(int x1, int y1, int x2, int y2, int color) {
  Draw_Line(x1, y1, x2, y1, color);
  Draw_Line(x1, y2, x2, y2, color);
  Draw_Line(x1, y1, x1, y2, color);
  Draw_Line(x2, y1, x2, y2, color);
}

void GraphicViewPortClass::Put_Pixel(int x, int y, int color) {
  if (!impl_->Contains(x, y)) {
    return;
  }
  impl_->buffer->Put_Pixel(x, y, Normalize_Color(color));
}

int GraphicViewPortClass::Get_Pixel(int x, int y) const {
  if (!impl_->Contains(x, y)) {
    return 0;
  }
  return impl_->buffer->Get_Pixel(x, y);
}

bool GraphicViewPortClass::Contains(int x, int y) const {
  return impl_->Contains(x, y);
}

int GraphicViewPortClass::Get_XPos() const {
  return impl_->left;
}

int GraphicViewPortClass::Get_YPos() const {
  return impl_->top;
}

int GraphicViewPortClass::Get_Width() const {
  return impl_->width;
}

int GraphicViewPortClass::Get_Height() const {
  return impl_->height;
}

GraphicBufferClass* GraphicViewPortClass::Get_Graphic_Buffer() const {
  return impl_->buffer;
}

GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page) {
  GraphicViewPortClass* old_page = LogicPage;
  LogicPage = &page;
  return old_page;
}

bool Set_Video_Mode(int mode) {
  switch (mode) {
    case RESET_MODE:
      Resize_Framebuffers(kDefaultScreenWidth, kDefaultScreenHeight);
      return true;
    case MCGA_MODE:
      Resize_Framebuffers(kMcgaWidth, kMcgaHeight);
      return true;
    default:
      break;
  }
  return false;
}

bool Set_Video_Mode(void* window, int width, int height, int bits_per_pixel) {
  (void)window;
  (void)bits_per_pixel;
  if (width <= 0 || height <= 0) {
    return false;
  }
  Resize_Framebuffers(width, height);
  return true;
}

struct WWMouseClass::Impl {
  GraphicViewPortClass* default_page = nullptr;
  int cursor_width = 1;
  int cursor_height = 1;
  bool cursor_drawn = false;
  GraphicViewPortClass* drawn_page = nullptr;
  std::vector<CursorPixel> saved_pixels;
};

WWMouseClass::WWMouseClass() : WWMouseClass(nullptr, 1, 1) {}

WWMouseClass::WWMouseClass(GraphicViewPortClass* page, int width, int height)
    : impl_(std::make_unique<Impl>()) {
  impl_->default_page = page;
  impl_->cursor_width = std::max(width, 1);
  impl_->cursor_height = std::max(height, 1);
}

WWMouseClass::WWMouseClass(WWMouseClass&&) noexcept = default;
WWMouseClass& WWMouseClass::operator=(WWMouseClass&&) noexcept = default;

WWMouseClass::~WWMouseClass() = default;

void WWMouseClass::Draw_Mouse(GraphicViewPortClass* page) {
  GraphicViewPortClass* target = page ? page : impl_->default_page;
  if (!target) {
    return;
  }

  Erase_Mouse(target, true);

  const int mouse_x = Get_Mouse_X();
  const int mouse_y = Get_Mouse_Y();
  const int half_width = std::max(impl_->cursor_width / 2, 1);
  const int half_height = std::max(impl_->cursor_height / 2, 1);

  impl_->saved_pixels.clear();
  impl_->saved_pixels.reserve(
      static_cast<std::size_t>(half_width + half_height) * 4);

  auto SavePixel = [&](int px, int py) {
    if (!target->Contains(px, py)) {
      return;
    }
    impl_->saved_pixels.push_back(
        {px, py, static_cast<std::uint8_t>(target->Get_Pixel(px, py))});
  };

  for (int dx = -half_width; dx <= half_width; ++dx) {
    SavePixel(mouse_x + dx, mouse_y);
  }
  for (int dy = -half_height; dy <= half_height; ++dy) {
    SavePixel(mouse_x, mouse_y + dy);
  }

  for (int dx = -half_width; dx <= half_width; ++dx) {
    target->Put_Pixel(mouse_x + dx, mouse_y, kMouseOutlineColor);
  }
  for (int dy = -half_height; dy <= half_height; ++dy) {
    target->Put_Pixel(mouse_x, mouse_y + dy, kMouseOutlineColor);
  }
  target->Put_Pixel(mouse_x, mouse_y, kMouseCenterColor);

  impl_->cursor_drawn = true;
  impl_->drawn_page = target;
}

void WWMouseClass::Erase_Mouse(GraphicViewPortClass* page, bool force) {
  if (!impl_->cursor_drawn || !impl_->drawn_page) {
    return;
  }
  GraphicViewPortClass* target = impl_->drawn_page;
  if (!force && page && page != target) {
    return;
  }

  for (const CursorPixel& pixel : impl_->saved_pixels) {
    target->Put_Pixel(pixel.x, pixel.y, pixel.color);
  }
  impl_->saved_pixels.clear();
  impl_->cursor_drawn = false;
  impl_->drawn_page = nullptr;
}

void WWMouseClass::Clear_Cursor_Clip() {
  g_mouse_clip.enabled = false;
}

void WWMouseClass::Set_Cursor_Clip() {
  if (!LogicPage) {
    g_mouse_clip.enabled = false;
    return;
  }
  g_mouse_clip.enabled = true;
  g_mouse_clip.left = LogicPage->Get_XPos();
  g_mouse_clip.top = LogicPage->Get_YPos();
  g_mouse_clip.right = LogicPage->Get_XPos() + LogicPage->Get_Width() - 1;
  g_mouse_clip.bottom = LogicPage->Get_YPos() + LogicPage->Get_Height() - 1;
  Update_Mouse_Position(Get_Mouse_X(), Get_Mouse_Y());
}

struct WWKeyboardClass::Impl {
  void Set_Key_State(int key, bool pressed) {
    key = Normalize_Key(key);
    if (pressed) {
      held_keys.insert(key);
    } else {
      held_keys.erase(key);
    }
  }

  bool Is_Key_Down(int key) const {
    key = Normalize_Key(key);
    return held_keys.find(key) != held_keys.end();
  }

  void Queue_Mouse_Button(int key, bool pressed, WWKeyboardClass& owner, int x, int y) {
    bool* state_flag = nullptr;
    if (key == KN_LMOUSE) {
      state_flag = &left_button_down;
    } else if (key == KN_RMOUSE) {
      state_flag = &right_button_down;
    }
    if (state_flag && *state_flag == pressed) {
      return;
    }
    if (state_flag) {
      *state_flag = pressed;
    }
    Sync_Mouse_Event_Position(owner, x, y);
    const int event = pressed ? key : (key | KN_RLSE_BIT);
    key_queue.Push(event);
    Set_Key_State(key, pressed);
  }

  KeyQueue key_queue;
  bool left_button_down = false;
  bool right_button_down = false;
  std::unordered_set<int> held_keys;
};

WWKeyboardClass::WWKeyboardClass() : impl_(std::make_unique<Impl>()) {}

WWKeyboardClass::WWKeyboardClass(WWKeyboardClass&&) noexcept = default;

WWKeyboardClass& WWKeyboardClass::operator=(WWKeyboardClass&&) noexcept = default;

WWKeyboardClass::~WWKeyboardClass() = default;

int WWKeyboardClass::Get() {
  if (!impl_) {
    return KN_NONE;
  }
  return impl_->key_queue.Pop();
}

int WWKeyboardClass::Check() const {
  if (!impl_) {
    return KN_NONE;
  }
  return impl_->key_queue.Peek();
}

void WWKeyboardClass::Clear() {
  if (!impl_) {
    return;
  }
  impl_->key_queue.Clear();
  impl_->held_keys.clear();
  impl_->left_button_down = false;
  impl_->right_button_down = false;
}

void WWKeyboardClass::Stuff(int key) {
  if (!impl_) {
    return;
  }
  impl_->key_queue.Push(key);
  impl_->Set_Key_State(key, (key & KN_RLSE_BIT) == 0);
}

bool WWKeyboardClass::Down(int key) const {
  if (!impl_) {
    return false;
  }
  return impl_->Is_Key_Down(key);
}

void WWKeyboardClass::Message_Handler(HWND, unsigned int, WPARAM, LPARAM) {}

WWKeyboardClass Kbd;
WWKeyboardClass* _Kbd = &Kbd;

void const* Set_Current_Font(void const* font) {
  const void* previous = g_current_font;
  g_current_font = font;
  return previous;
}

void const* Get_Current_Font() {
  return g_current_font;
}

void Set_Gradient_Font_6(void const* font) {
  g_grad_font6 = font;
}

void const* Get_Gradient_Font_6() {
  return g_grad_font6;
}

int Get_Mouse_X() {
  return g_mouse_position.x;
}

int Get_Mouse_Y() {
  return g_mouse_position.y;
}

void Update_Mouse_Position(int x, int y) {
  if (g_mouse_clip.enabled) {
    x = ClampValue(x, g_mouse_clip.left, g_mouse_clip.right);
    y = ClampValue(y, g_mouse_clip.top, g_mouse_clip.bottom);
  }
  g_mouse_position.x = x;
  g_mouse_position.y = y;
}

int Get_Key_Num() {
  return Kbd.Get();
}

int Check_Key_Num() {
  return Kbd.Check();
}

int KN_To_KA(int key) {
  return static_cast<int>(NormalizeAscii(key));
}

void Clear_KeyBuffer() {
  Kbd.Clear();
}

void Stuff_Key_Num(int key) {
  Kbd.Stuff(key);
}

int Key_Down(int key) {
  return Kbd.Down(key) ? 1 : 0;
}

void Platform_Update_Mouse_State(const PlatformMouseState& state) {
  Update_Mouse_Position(state.x, state.y);
  if (!Kbd.impl_) {
    return;
  }
  Kbd.impl_->Queue_Mouse_Button(KN_LMOUSE, state.left_button_down, Kbd, state.x,
                                state.y);
  Kbd.impl_->Queue_Mouse_Button(KN_RMOUSE, state.right_button_down, Kbd, state.x,
                                state.y);
}

void Platform_Queue_Key_Event(int key, bool pressed) {
  if (!Kbd.impl_) {
    return;
  }
  const int normalized = Normalize_Key(key);
  const int event = pressed ? normalized : (normalized | KN_RLSE_BIT);
  Kbd.impl_->key_queue.Push(event);
  Kbd.impl_->Set_Key_State(normalized, pressed);
}

int Desired_Facing256(int x1, int y1, int x2, int y2) {
  const int dx = x2 - x1;
  const int dy = y2 - y1;
  if (dx == 0 && dy == 0) {
    return 0;
  }

  double angle = std::atan2(static_cast<double>(dy), static_cast<double>(dx));
  angle /= kTau;
  if (angle < 0.0) {
    angle += 1.0;
  }

  const int mask = static_cast<int>(std::numeric_limits<std::uint8_t>::max());
  const int value = static_cast<int>(std::lround(angle * 256.0)) & mask;
  return value;
}

int Desired_Facing8(int x1, int y1, int x2, int y2) {
  const int facing256 = Desired_Facing256(x1, y1, x2, y2);
  return static_cast<int>(((facing256 + 16) & 0xFF) >> 5);
}

char const* Extract_String(char const* table, int index) {
  if (!table || index < 0) {
    return kEmptyString;
  }

  const char* current = table;
  while (index > 0 && *current) {
    while (*current++) {
    }
    --index;
  }

  if (!*current) {
    return kEmptyString;
  }
  return current;
}
