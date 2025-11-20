#include "function.h"
#include "edit.h"

#include <algorithm>
#include <cctype>
#include <cstring>

namespace {
constexpr char kCursorGlyph[] = "_";
constexpr int kVirtualKeyBit = 0x0100;
constexpr KeyASCIIType kAsciiReturn = '\r';
constexpr KeyASCIIType kAsciiBackspace = 0x08;

bool Has_Mouse_Button_Release(unsigned flags) {
  return (flags &
          (GadgetClass::LEFTRELEASE | GadgetClass::RIGHTRELEASE)) != 0;
}
}  // namespace

EditClass::EditClass(int id, char* text, int max_len, TextPrintType flags, int x, int y,
                     int w, int h, EditStyle style)
    : ControlClass(id, x, y, w, h, LEFTPRESS), TextFlags(flags), EditFlags(style), String(text) {
  Color = CC_GREEN;
  Set_Text(text, max_len);

  if (w == -1 || h == -1) {
    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, TextFlags);
    if (h == -1) {
      Height = FontHeight + 2;
    }
    if (w == -1) {
      const int baseline =
          (Length > 0) ? String_Pixel_Width(String) : Char_Pixel_Width('X') * (MaxLength + 1);
      Width = baseline + 6;
    }
  }
  IsReadOnly = 0;
}

EditClass::~EditClass() {
  if (Has_Focus()) {
    Clear_Focus();
  }
}

int EditClass::Draw_Me(int forced) {
  if (!ControlClass::Draw_Me(forced)) {
    return false;
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
  }

  Draw_Background();
  Draw_Text(String);

  if (LogicPage == &SeenBuff) {
    Conditional_Show_Mouse();
  }
  return true;
}

void EditClass::Set_Text(char* text, int max_len) {
  String = text;
  MaxLength = std::max(max_len - 1, 0);
  Length = static_cast<int>(std::strlen(String));
  if (Length > MaxLength) {
    Length = MaxLength;
    String[Length] = '\0';
  }
  Flag_To_Redraw();
}

int EditClass::Action(unsigned flags, KeyNumType& key) {
  if (IsReadOnly) {
    return false;
  }

  if ((flags & LEFTPRESS) != 0) {
    flags &= ~LEFTPRESS;
    Set_Focus();
    Flag_To_Redraw();
  }

  if ((flags & KEYBOARD) && Has_Focus()) {
    if (key == KN_ESC) {
      Clear_Focus();
      flags = 0;
    } else {
      const KeyASCIIType ascii =
          static_cast<KeyASCIIType>(Keyboard::To_ASCII(key) & 0x00FF);
      const bool numeric_vk = (key & kVirtualKeyBit) && ascii >= '0' && ascii <= '9';

      if (numeric_vk) {
        key &= ~kVirtualKeyBit;
        if (!Has_Mouse_Button_Release(flags) && Handle_Key(ascii)) {
          flags &= ~KEYBOARD;
          key = KN_NONE;
        }
      } else {
        const bool printable =
            ((key & kVirtualKeyBit) == 0 && ascii >= ' ' && ascii <= 127);
        const bool special = (ascii == kAsciiReturn || ascii == kAsciiBackspace);
        if ((printable || special) && !Has_Mouse_Button_Release(flags) &&
            Handle_Key(Keyboard::To_ASCII(key))) {
          flags &= ~KEYBOARD;
          key = KN_NONE;
        } else if (!printable && !special) {
          flags &= ~KEYBOARD;
          key = KN_NONE;
        }
      }
    }
  }

  return ControlClass::Action(flags, key);
}

void EditClass::Draw_Background() {
  Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_BOX, true);
}

void EditClass::Draw_Text(char const* text) {
  const bool has_focus = Has_Focus();
  const bool fits_cursor =
      (static_cast<int>(String_Pixel_Width(text)) +
       String_Pixel_Width(kCursorGlyph)) < (Width - 2);

  const bool use_grad_font = false;
  if (use_grad_font) {
    TextPrintType draw_flags = TextFlags;
    if (has_focus) {
      draw_flags = static_cast<TextPrintType>(draw_flags | TPF_BRIGHT_COLOR);
    }

    Conquer_Clip_Text_Print(text, X + 1, Y + 1, Color, TBLACK, draw_flags, Width - 2);
    if (has_focus && Length < MaxLength && fits_cursor) {
      Conquer_Clip_Text_Print(kCursorGlyph, X + 1 + String_Pixel_Width(text), Y + 1,
                              Color, TBLACK, draw_flags);
    }
  } else {
    const int base_color = has_focus ? BLUE : WHITE;
    Conquer_Clip_Text_Print(text, X + 1, Y + 1, base_color, TBLACK, TextFlags, Width - 2);
    if (has_focus && Length < MaxLength && fits_cursor) {
      Conquer_Clip_Text_Print(kCursorGlyph, X + 1 + String_Pixel_Width(text), Y + 1,
                              BLUE, TBLACK, TextFlags);
    }
  }
}

bool EditClass::Handle_Key(KeyASCIIType ascii) {
  const unsigned char ch = static_cast<unsigned char>(ascii);

  switch (ascii) {
    case 0:
      return true;
    case kAsciiReturn:
      Clear_Focus();
      return false;
    case kAsciiBackspace:
      if (Length > 0) {
        --Length;
        String[Length] = '\0';
        Flag_To_Redraw();
      }
      return true;
    default:
      break;
  }

  if ((String_Pixel_Width(String) + Char_Pixel_Width(ascii)) >= (Width - 2)) {
    return true;
  }
  if (Length >= MaxLength) {
    return true;
  }
  if ((!std::isgraph(ch) && ascii != ' ') || (ascii == ' ' && Length == 0)) {
    return true;
  }

  if ((EditFlags & UPPERCASE) && std::isalpha(ch)) {
    ascii = static_cast<KeyASCIIType>(std::toupper(ch));
  }

  const bool numeric_allowed = (EditFlags & NUMERIC) != 0;
  const bool alpha_allowed = (EditFlags & ALPHA) != 0;
  const bool misc_allowed = (EditFlags & MISC) != 0;

  if ((!numeric_allowed && std::isdigit(ch)) ||
      (!alpha_allowed && std::isalpha(ch)) ||
      (!misc_allowed && !std::isalnum(ch) && ascii != ' ')) {
    return true;
  }

  String[Length++] = static_cast<char>(ascii);
  String[Length] = '\0';
  Flag_To_Redraw();
  return true;
}
