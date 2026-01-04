// Movie playback (VQA).
// This port implements the classic (8-bit) Westwood VQA decoder used by the
// original game assets and renders frames into the existing 8-bit software
// pages managed by the SDL runtime.

#include "legacy/debug.h"
#include "legacy/audio.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"

#include <SDL.h>

#include "vqa_decoder.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>

namespace {

std::string Trim_Whitespace(char const* text) {
  if (!text) return {};
  while (*text && std::isspace(static_cast<unsigned char>(*text))) {
    ++text;
  }
  std::string value(text);
  while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
    value.pop_back();
  }
  return value;
}

bool Is_No_Movie_Sentinel(std::string const& name) {
  if (name.empty()) return true;
  if (name.size() == 1 && (name[0] == 'x' || name[0] == 'X')) return true;
  std::string upper = name;
  std::transform(upper.begin(), upper.end(), upper.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return upper == "X" || upper == "X.VQA";
}

bool Movie_Should_Skip() {
  if (ReadyToQuit) return true;
  if (Keyboard::Check()) return true;
  return false;
}

std::string Normalize_Vqa_Name(char const* name) {
  const std::string trimmed = Trim_Whitespace(name);
  if (Is_No_Movie_Sentinel(trimmed)) return {};

  std::string result(trimmed);
  if (result.empty()) return result;
  const auto dot = result.find_last_of('.');
  if (dot == std::string::npos) {
    result += ".VQA";
    return result;
  }
  std::string ext = result.substr(dot);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::toupper(c); });
  if (ext != ".VQA") result += ".VQA";
  return result;
}

void Fill_View(GraphicViewPortClass& view, std::uint8_t color) {
  const int width = view.Get_Width();
  const int height = view.Get_Height();
  if (width <= 0 || height <= 0) return;
  const GraphicBufferClass* buffer = view.Get_Graphic_Buffer();
  const int stride = buffer ? buffer->Get_Width() : 0;
  auto* base = static_cast<std::uint8_t*>(view.Get_Offset());
  if (!base || stride <= 0) return;
  for (int y = 0; y < height; ++y) {
    std::memset(base + static_cast<std::size_t>(y) * stride, color, static_cast<std::size_t>(width));
  }
}

void Blit_Scaled_Centered(GraphicViewPortClass& dest, const VqaDecoder::Frame& frame) {
  const int dst_w = dest.Get_Width();
  const int dst_h = dest.Get_Height();
  const GraphicBufferClass* buffer = dest.Get_Graphic_Buffer();
  const int stride = buffer ? buffer->Get_Width() : 0;
  auto* dst_base = static_cast<std::uint8_t*>(dest.Get_Offset());
  if (!dst_base || dst_w <= 0 || dst_h <= 0 || stride <= 0) return;

  const int src_w = static_cast<int>(frame.width);
  const int src_h = static_cast<int>(frame.height);
  if (src_w <= 0 || src_h <= 0) return;
  if (frame.indices.size() < static_cast<std::size_t>(src_w) * static_cast<std::size_t>(src_h)) return;

  const int scale_x = dst_w / src_w;
  const int scale_y = dst_h / src_h;
  const int scale = std::max(1, std::min(scale_x, scale_y));

  const int out_w = src_w * scale;
  const int out_h = src_h * scale;
  const int offset_x = (dst_w - out_w) / 2;
  const int offset_y = (dst_h - out_h) / 2;

  Fill_View(dest, 0);

  for (int y = 0; y < src_h; ++y) {
    const std::uint8_t* src_row = frame.indices.data() + static_cast<std::size_t>(y) * src_w;
    for (int sy = 0; sy < scale; ++sy) {
      const int dy = offset_y + y * scale + sy;
      if (dy < 0 || dy >= dst_h) continue;
      auto* dst_row = dst_base + static_cast<std::size_t>(dy) * stride;
      for (int x = 0; x < src_w; ++x) {
        const std::uint8_t color = src_row[x];
        const int dx0 = offset_x + x * scale;
        if (dx0 >= dst_w) continue;
        if (dx0 + scale <= 0) continue;
        const int clipped_dx = std::max(0, dx0);
        const int clipped_w = std::min(dst_w, dx0 + scale) - clipped_dx;
        if (clipped_w <= 0) continue;
        std::memset(dst_row + clipped_dx, color, static_cast<std::size_t>(clipped_w));
      }
    }
  }
}

}  // namespace

void Play_Movie(char const* name, ThemeType theme, bool clrscrn) {
  const std::string filename = Normalize_Vqa_Name(name);
  if (filename.empty()) return;

  /*
  ** Don't play movies in editor mode.
  */
  if (Debug_Map) {
    return;
  }

  /*
  ** Don't play movies in multiplayer mode.
  */
  if (GameToPlay != GAME_NORMAL) {
    return;
  }

  VqaDecoder decoder;
  if (!decoder.Open(filename)) {
    std::string dbg = "Play_Movie: unable to open '";
    dbg += filename;
    dbg += "'\n";
    CCDebugString(dbg.c_str());
    return;
  }

  const std::uint16_t frames = decoder.Frame_Count();
  const std::uint8_t fps = decoder.Frame_Rate() ? decoder.Frame_Rate() : 15;
  const int frame_ms = std::max(1, 1000 / static_cast<int>(fps));

  // Mirror the legacy flow: fade/clear before playback unless the caller
  // requested the VQA screen be preserved between transitions.
  const int mouse_state_before = Get_Mouse_State();
  Hide_Mouse();
  Theme.Queue_Song(theme);
  if (PreserveVQAScreen == 0) {
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    VisiblePage.Clear();
    std::memset(BlackPalette, 0x01, 768);
    Set_Palette(BlackPalette);
    std::memset(BlackPalette, 0x00, 768);
  }
  PreserveVQAScreen = 0;
  Keyboard::Clear();

  VqaDecoder::Frame frame;
  using clock = std::chrono::steady_clock;
  auto next_tick = clock::now();
  bool started_audio = false;
  const auto old_logic_page = Set_Logic_Page(&SeenBuff);
  Brokeout = false;

  for (std::uint16_t i = 0; i < frames; ++i) {
    if (!decoder.Decode_Frame(i, frame)) break;

    Set_Palette(frame.palette.data());
    if (!frame.audio_pcm.empty() && frame.audio_rate > 0 && frame.audio_channels > 0) {
      SDL_AudioFormat fmt = (frame.audio_bits >= 16) ? AUDIO_S16LSB : AUDIO_U8;
      if (!started_audio) {
        Movie_Audio_Begin(Options.Volume, fmt, frame.audio_channels, frame.audio_rate);
        started_audio = true;
      }
      Movie_Audio_Push(frame.audio_pcm.data(), frame.audio_pcm.size());
    }

    Blit_Scaled_Centered(SeenBuff, frame);
    Call_Back();
    if (Movie_Should_Skip()) {
      Brokeout = true;
      break;
    }

    next_tick += std::chrono::milliseconds(frame_ms);
    const auto now = clock::now();
    if (next_tick > now) {
      const auto delay_ms = std::chrono::duration_cast<std::chrono::milliseconds>(next_tick - now).count();
      if (delay_ms > 0) SDL_Delay(static_cast<Uint32>(delay_ms));
    } else {
      next_tick = now;
    }
  }

  if (started_audio) {
    Movie_Audio_End();
  }

  Set_Logic_Page(old_logic_page);

  if (Brokeout) {
    clrscrn = true;
    VisiblePage.Clear();
    Brokeout = false;
  }

  // When requested, clear to black after playback (legacy semantics).
  if (clrscrn) {
    VisiblePage.Clear();
    std::memset(BlackPalette, 0x01, 768);
    Set_Palette(BlackPalette);
    std::memset(BlackPalette, 0x00, 768);
    Set_Palette(BlackPalette);
  }

  while (Get_Mouse_State() > mouse_state_before) {
    Show_Mouse();
  }
}
