// Movie playback (VQA).
// This port implements the classic (8-bit) Westwood VQA decoder used by the
// original game assets and renders frames into the existing 8-bit software
// pages managed by the SDL runtime.

#include <SDL.h>

#include "legacy/debug.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"

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
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) return true;
    if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) return true;
  }
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

}  // namespace

void Play_Movie(char const* name, ThemeType /*theme*/, bool clrscrn) {
  const std::string filename = Normalize_Vqa_Name(name);
  if (filename.empty()) return;

  VqaDecoder decoder;
  if (!decoder.Open(filename)) {
    std::string dbg = "Play_Movie: unable to open '";
    dbg += filename;
    dbg += "'\n";
    CCDebugString(dbg.c_str());
    return;
  }

  if (clrscrn) {
    VisiblePage.Clear();
    GScreenClass::Blit_Display();
  }

  const std::uint16_t frames = decoder.Frame_Count();
  const std::uint8_t fps = decoder.Frame_Rate() ? decoder.Frame_Rate() : 15;
  const int frame_ms = std::max(1, 1000 / static_cast<int>(fps));

  const int dst_w = VisiblePage.Get_Width();
  const int dst_h = VisiblePage.Get_Height();
  std::uint8_t* dst = VisiblePage.Get_Buffer();
  if (!dst || dst_w <= 0 || dst_h <= 0) return;

  VqaDecoder::Frame frame;
  using clock = std::chrono::steady_clock;
  auto next_tick = clock::now();

  for (std::uint16_t i = 0; i < frames; ++i) {
    if (Movie_Should_Skip()) break;
    if (!decoder.Decode_Frame(i, frame)) break;

    Set_Palette(frame.palette.data());

    const int src_w = static_cast<int>(frame.width);
    const int src_h = static_cast<int>(frame.height);
    const int copy_w = std::min(dst_w, src_w);
    const int copy_h = std::min(dst_h, src_h);

    for (int y = 0; y < copy_h; ++y) {
      const std::uint8_t* src_row = frame.indices.data() + static_cast<std::size_t>(y) * src_w;
      std::uint8_t* dst_row = dst + static_cast<std::size_t>(y) * dst_w;
      std::memcpy(dst_row, src_row, static_cast<std::size_t>(copy_w));
    }

    GScreenClass::Blit_Display();

    next_tick += std::chrono::milliseconds(frame_ms);
    const auto now = clock::now();
    if (next_tick > now) {
      const auto delay_ms = std::chrono::duration_cast<std::chrono::milliseconds>(next_tick - now).count();
      if (delay_ms > 0) SDL_Delay(static_cast<Uint32>(delay_ms));
    } else {
      next_tick = now;
    }
  }
}
