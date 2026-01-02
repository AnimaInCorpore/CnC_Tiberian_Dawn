// SDL mixer-backed playback for legacy sound effect and EVA calls.

#include <SDL.h>

#include "function.h"
#include "legacy/audio.h"
#include "legacy/defines.h"
#include "legacy/externs.h"
#include "legacy/mixfile.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace {

struct AudChunkHeader {
  std::uint16_t compressed_size;
  std::uint16_t output_size;
  std::uint32_t magic;
};

enum class AudFormat : std::uint8_t {
  WestwoodCompressed = 1,
  ImaAdpcm = 99,
};

struct DecodedAudio {
  std::vector<std::uint8_t> pcm;
  int sample_rate = 0;
  SDL_AudioFormat format = 0;
  std::uint8_t channels = 0;
};

struct Voice {
  const void* key = nullptr;
  int handle = 0;
  int priority = 0;
  int volume = 0;  // 0..255
  int pan = 0;     // -32767..32767
  bool loop = false;
  bool is_score = false;
  int fade_ticks_remaining = 0;
  int fade_ticks_total = 0;
  int fade_start_volume = 0;
  std::size_t pos = 0;
};

std::mutex g_cache_mutex;
std::unordered_map<const void*, DecodedAudio> g_cache;
std::vector<Voice> g_voices;
int g_next_handle = 1;
std::atomic<int> g_score_volume{255};

inline int ClampInt(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }
constexpr int kMaxVoices = 32;

struct StereoScales {
  int left = 255;
  int right = 255;
};

static StereoScales Compute_Pan_Scales(int volume, int pan) {
  StereoScales scales{};
  const int vol = ClampInt(volume, 0, 255);
  const int clamped_pan = ClampInt(pan, -32767, 32767);

  // Linear panning in legacy integer space: -32767 is left, 0 is center, 32767 is right.
  // Convert into per-channel scales (0..255) and apply the overall volume.
  const int left_scale = (255 * (32767 - clamped_pan)) / (2 * 32767);
  const int right_scale = (255 * (32767 + clamped_pan)) / (2 * 32767);
  scales.left = (vol * left_scale) / 255;
  scales.right = (vol * right_scale) / 255;
  return scales;
}

static bool Read_Aud_Header(const std::uint8_t* data,
                            int& sample_rate,
                            std::uint32_t& data_size,
                            std::uint32_t& output_size,
                            std::uint8_t& flags,
                            AudFormat& fmt,
                            std::size_t& data_offset) {
  if (!data) return false;
  // AUD header is 12 bytes; original code sometimes treated buffers as opaque pointers.
  sample_rate = static_cast<int>(data[0] | (data[1] << 8));
  data_size = static_cast<std::uint32_t>(data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24));
  output_size = static_cast<std::uint32_t>(data[6] | (data[7] << 8) | (data[8] << 16) | (data[9] << 24));
  flags = data[10];
  fmt = static_cast<AudFormat>(data[11]);
  data_offset = 12;
  (void)flags;
  return true;
}

static bool Read_Chunk_Header(const std::uint8_t* data, std::size_t size, std::size_t offset,
                              AudChunkHeader& out, std::size_t& next) {
  if (!data) return false;
  if (offset + 8 > size) return false;
  out.compressed_size = static_cast<std::uint16_t>(data[offset] | (data[offset + 1] << 8));
  out.output_size = static_cast<std::uint16_t>(data[offset + 2] | (data[offset + 3] << 8));
  out.magic = static_cast<std::uint32_t>(data[offset + 4] | (data[offset + 5] << 8) | (data[offset + 6] << 16) |
                                         (data[offset + 7] << 24));
  next = offset + 8;
  return out.magic == 0x0000DEAFu;
}

static void Decode_Westwood_Compressed(const std::uint8_t* input, std::size_t input_len,
                                       std::uint8_t* output, std::size_t output_len) {
  if (!input || !output || output_len == 0) return;
  if (input_len == output_len) {
    std::memcpy(output, input, output_len);
    return;
  }

  static constexpr int kStep2[4] = {-2, -1, 0, 1};
  static constexpr int kStep4[16] = {-9, -8, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8};

  int sample = 0x80;
  std::size_t r = 0;
  std::size_t w = 0;

  while (r < input_len && w < output_len) {
    const std::uint8_t header = input[r++];
    std::uint8_t count = static_cast<std::uint8_t>(header & 0x3Fu);
    const std::uint8_t mode = static_cast<std::uint8_t>(header >> 6);

    switch (mode) {
      case 0:
        for (count++; count > 0 && r < input_len; count--) {
          const std::uint8_t code = input[r++];
          for (int s = 0; s < 4 && w < output_len; ++s) {
            sample = ClampInt(sample + kStep2[(code >> (s * 2)) & 0x03], 0, 255);
            output[w++] = static_cast<std::uint8_t>(sample);
          }
        }
        break;
      case 1:
        for (count++; count > 0 && r < input_len; count--) {
          const std::uint8_t code = input[r++];
          sample = ClampInt(sample + kStep4[(code >> 0) & 0x0F], 0, 255);
          if (w < output_len) output[w++] = static_cast<std::uint8_t>(sample);
          sample = ClampInt(sample + kStep4[(code >> 4) & 0x0F], 0, 255);
          if (w < output_len) output[w++] = static_cast<std::uint8_t>(sample);
        }
        break;
      case 2:
        if ((count & 0x20u) != 0) {
          std::int8_t delta = static_cast<std::int8_t>(static_cast<std::int8_t>(count << 3) >> 3);
          sample = ClampInt(sample + delta, 0, 255);
          if (w < output_len) output[w++] = static_cast<std::uint8_t>(sample);
        } else {
          for (count++; count > 0 && r < input_len && w < output_len; count--) {
            output[w++] = input[r++];
          }
          if (r > 0) sample = input[r - 1];
        }
        break;
      default:
        for (count++; count > 0 && w < output_len; count--) {
          output[w++] = static_cast<std::uint8_t>(sample);
        }
        break;
    }
  }
}

static std::int16_t Decode_Ima_Sample(std::uint8_t code, int& index, int& current) {
  static constexpr int kIndexAdjust[8] = {-1, -1, -1, -1, 2, 4, 6, 8};
  static constexpr int kStepTable[89] = {
      7,     8,     9,     10,    11,    12,    13,    14,    16,    17,    19,    21,    23,    25,    28,
      31,    34,    37,    41,    45,    50,    55,    60,    66,    73,    80,    88,    97,    107,   118,
      130,   143,   157,   173,   190,   209,   230,   253,   279,   307,   337,   371,   408,   449,   494,
      544,   598,   658,   724,   796,   876,   963,   1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
      2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,  7132,  7845,  8630,
      9493,  10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};

  const bool sign = (code & 8u) != 0;
  code &= 7u;

  int delta = kStepTable[index] * static_cast<int>(code) / 4 + kStepTable[index] / 8;
  if (sign) delta = -delta;
  current += delta;
  current = ClampInt(current, -32768, 32767);

  index += kIndexAdjust[code];
  index = ClampInt(index, 0, 88);
  return static_cast<std::int16_t>(current);
}

static bool Decode_Aud_To_Device_Pcm(const void* key, std::size_t key_size, DecodedAudio& decoded) {
  const auto* src = static_cast<const std::uint8_t*>(key);
  if (!src) return false;
  if (key_size < 12) return false;

  SDL_AudioSpec const* spec = Audio_Get_Spec();
  if (!spec || spec->freq <= 0) return false;

  int sample_rate = 0;
  std::uint32_t data_size = 0;
  std::uint32_t output_size = 0;
  std::uint8_t flags = 0;
  AudFormat format = AudFormat::WestwoodCompressed;
  std::size_t offset = 0;
  if (!Read_Aud_Header(src, sample_rate, data_size, output_size, flags, format, offset)) return false;
  if (offset > key_size) return false;

  static constexpr std::uint32_t kMaxAudData = 32u * 1024u * 1024u;
  static constexpr std::uint32_t kMaxAudOutput = 64u * 1024u * 1024u;
  if (sample_rate < 1000 || sample_rate > 96000) return false;
  if (data_size == 0 || output_size == 0) return false;
  if (data_size > kMaxAudData || output_size > kMaxAudOutput) return false;
  if (static_cast<std::size_t>(data_size) > key_size - offset) return false;

  const bool want_s16 = (spec->format == AUDIO_S16SYS);
  const bool want_u8 = (spec->format == AUDIO_U8);
  if (!want_s16 && !want_u8) return false;

  decoded.sample_rate = spec->freq;
  decoded.format = spec->format;
  decoded.channels = spec->channels;

  // Decode into mono stream first.
  std::vector<std::uint8_t> mono;
  mono.reserve(output_size);

  std::size_t cursor = offset;
  std::uint32_t remaining = data_size;
  while (remaining >= 8) {
    AudChunkHeader chunk{};
    std::size_t chunk_data = 0;
    if (!Read_Chunk_Header(src, key_size, cursor, chunk, chunk_data)) break;
    cursor = chunk_data;
    if (chunk.compressed_size > remaining) break;
    if (cursor + chunk.compressed_size > key_size) break;

    const std::uint8_t* chunk_input = src + cursor;
    cursor += chunk.compressed_size;
    remaining -= static_cast<std::uint32_t>(8 + chunk.compressed_size);

    if (format == AudFormat::WestwoodCompressed) {
      const std::size_t out_start = mono.size();
      mono.resize(out_start + chunk.output_size);
      Decode_Westwood_Compressed(chunk_input, chunk.compressed_size, mono.data() + out_start, chunk.output_size);
    } else if (format == AudFormat::ImaAdpcm) {
      int current = 0;
      int index = 0;
      for (std::size_t n = 0; n < chunk.compressed_size; ++n) {
        const std::uint8_t b = chunk_input[n];
        const std::int16_t s0 = Decode_Ima_Sample(b & 0x0F, index, current);
        mono.push_back(static_cast<std::uint8_t>(s0 & 0xFF));
        mono.push_back(static_cast<std::uint8_t>((s0 >> 8) & 0xFF));
        const std::int16_t s1 = Decode_Ima_Sample(static_cast<std::uint8_t>(b >> 4), index, current);
        mono.push_back(static_cast<std::uint8_t>(s1 & 0xFF));
        mono.push_back(static_cast<std::uint8_t>((s1 >> 8) & 0xFF));
      }
    } else {
      break;
    }
  }

  if (mono.empty()) return false;

  // Convert sample rate if needed (SDL provides a converter).
  const SDL_AudioFormat src_fmt = (format == AudFormat::ImaAdpcm) ? AUDIO_S16SYS : AUDIO_U8;
  const int src_channels = 1;

  SDL_AudioCVT cvt;
  if (SDL_BuildAudioCVT(&cvt, src_fmt, src_channels, sample_rate, spec->format, spec->channels, spec->freq) < 0) {
    return false;
  }

  decoded.pcm = std::move(mono);
  if (cvt.needed) {
    cvt.len = static_cast<int>(decoded.pcm.size());
    decoded.pcm.resize(static_cast<std::size_t>(cvt.len * cvt.len_mult));
    cvt.buf = decoded.pcm.data();
    if (SDL_ConvertAudio(&cvt) < 0) {
      return false;
    }
    decoded.pcm.resize(static_cast<std::size_t>(cvt.len_cvt));
  }
  return true;
}

static int Mix_Voice(const Voice& voice, std::uint8_t* stream, int len) {
  auto it = g_cache.find(voice.key);
  if (it == g_cache.end()) return 0;
  const DecodedAudio& decoded = it->second;
  if (decoded.pcm.empty()) return 0;

  const int bytes_per_sample = (decoded.format == AUDIO_S16SYS) ? 2 : 1;
  const int frame_bytes = bytes_per_sample * decoded.channels;
  if (frame_bytes <= 0) return 0;

  std::size_t pos = voice.pos;
  const std::size_t available = decoded.pcm.size();
  const std::size_t want = static_cast<std::size_t>(len);
  if (pos >= available) return 0;

  const std::size_t to_mix = std::min(want, available - pos);
  const std::uint8_t* src = decoded.pcm.data() + pos;

  // Volume is 0..255; match legacy behavior where 255 ~= full scale.
  int effective_volume = voice.volume;
  if (voice.is_score) {
    effective_volume = (effective_volume * ClampInt(g_score_volume.load(std::memory_order_relaxed), 0, 255)) / 255;
  }
  const StereoScales scales = Compute_Pan_Scales(effective_volume, voice.pan);

  if (decoded.format == AUDIO_S16SYS) {
    auto* dst16 = reinterpret_cast<std::int16_t*>(stream);
    const auto* src16 = reinterpret_cast<const std::int16_t*>(src);
    const int samples = static_cast<int>(to_mix / 2);
    if (decoded.channels == 2) {
      for (int i = 0; i + 1 < samples; i += 2) {
        const int mixed_l = static_cast<int>(dst16[i]) + (static_cast<int>(src16[i]) * scales.left) / 255;
        const int mixed_r = static_cast<int>(dst16[i + 1]) + (static_cast<int>(src16[i + 1]) * scales.right) / 255;
        dst16[i] = static_cast<std::int16_t>(ClampInt(mixed_l, -32768, 32767));
        dst16[i + 1] = static_cast<std::int16_t>(ClampInt(mixed_r, -32768, 32767));
      }
    } else {
      const int vol = scales.left;
      for (int i = 0; i < samples; ++i) {
        const int mixed = static_cast<int>(dst16[i]) + (static_cast<int>(src16[i]) * vol) / 255;
        dst16[i] = static_cast<std::int16_t>(ClampInt(mixed, -32768, 32767));
      }
    }
  } else {
    if (decoded.channels == 2) {
      for (std::size_t i = 0; i + 1 < to_mix; i += 2) {
        const int s_l = static_cast<int>(src[i]) - 128;
        const int d_l = static_cast<int>(stream[i]) - 128;
        const int mixed_l = d_l + (s_l * scales.left) / 255;
        stream[i] = static_cast<std::uint8_t>(ClampInt(mixed_l + 128, 0, 255));

        const int s_r = static_cast<int>(src[i + 1]) - 128;
        const int d_r = static_cast<int>(stream[i + 1]) - 128;
        const int mixed_r = d_r + (s_r * scales.right) / 255;
        stream[i + 1] = static_cast<std::uint8_t>(ClampInt(mixed_r + 128, 0, 255));
      }
    } else {
      const int vol = scales.left;
      for (std::size_t i = 0; i < to_mix; ++i) {
        const int s = static_cast<int>(src[i]) - 128;
        const int d = static_cast<int>(stream[i]) - 128;
        const int mixed = d + (s * vol) / 255;
        stream[i] = static_cast<std::uint8_t>(ClampInt(mixed + 128, 0, 255));
      }
    }
  }

  return static_cast<int>(to_mix);
}

}  // namespace

void Set_Score_Vol(int volume) {
  g_score_volume.store(ClampInt(volume, 0, 255), std::memory_order_relaxed);
}

extern "C" void Audio_Mix_Callback(void*, Uint8* stream, int len) {
  if (!stream || len <= 0) return;
  SDL_AudioSpec const* spec = Audio_Get_Spec();
  if (!spec) return;

  // Clear buffer first (silence).
  if (spec->format == AUDIO_S16SYS) {
    std::memset(stream, 0, static_cast<std::size_t>(len));
  } else {
    std::memset(stream, 128, static_cast<std::size_t>(len));
  }

  std::lock_guard<std::mutex> lock(g_cache_mutex);
  for (auto it = g_voices.begin(); it != g_voices.end();) {
    const int mixed_bytes = Mix_Voice(*it, stream, len);
    it->pos += static_cast<std::size_t>(mixed_bytes);
    auto cache_it = g_cache.find(it->key);
    const std::size_t end = (cache_it != g_cache.end()) ? cache_it->second.pcm.size() : 0;
    if (end == 0) {
      it = g_voices.erase(it);
      continue;
    }
    if (it->pos >= end) {
      if (it->loop) {
        it->pos = 0;
      } else {
        it = g_voices.erase(it);
        continue;
      }
    } else {
      ++it;
    }
  }
}

static int Play_Aud_Ptr(const void* ptr, int volume, int priority, signed short pan, bool loop, bool is_score) {
  if (!ptr || Audio_Get_Device() == 0) return 0;

  std::lock_guard<std::mutex> lock(g_cache_mutex);
  auto it = g_cache.find(ptr);
  if (it == g_cache.end()) {
    std::size_t size = MixFileClass::Size_For_Pointer(ptr);
    if (size == 0 && ptr == SpeechBuffer) {
      size = static_cast<std::size_t>(SPEECH_BUFFER_SIZE);
    }
    if (size == 0) return 0;
    DecodedAudio decoded{};
    if (!Decode_Aud_To_Device_Pcm(ptr, size, decoded)) return 0;
    it = g_cache.emplace(ptr, std::move(decoded)).first;
  }

  Voice voice{};
  voice.key = ptr;
  voice.handle = g_next_handle++;
  voice.priority = priority;
  voice.volume = ClampInt(volume, 0, 255);
  voice.pan = pan;
  voice.loop = loop;
  voice.is_score = is_score;
  voice.fade_ticks_remaining = 0;
  voice.fade_ticks_total = 0;
  voice.fade_start_volume = voice.volume;
  voice.pos = 0;

  if (static_cast<int>(g_voices.size()) >= kMaxVoices) {
    auto lowest = std::min_element(g_voices.begin(), g_voices.end(),
                                  [](const Voice& a, const Voice& b) { return a.priority < b.priority; });
    if (lowest != g_voices.end() && priority > lowest->priority) {
      g_voices.erase(lowest);
    } else {
      return 0;
    }
  }

  g_voices.push_back(voice);
  return voice.handle;
}

int Play_Sample(void const* sample, int priority, int volume, int pan) {
  return Play_Aud_Ptr(sample, volume, priority, static_cast<signed short>(pan), false, false);
}

void Stop_Sample(int handle) {
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  g_voices.erase(std::remove_if(g_voices.begin(), g_voices.end(), [&](const Voice& v) { return v.handle == handle; }),
                 g_voices.end());
}

bool Is_Sample_Playing(void const* data) {
  if (!data) return false;
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  for (const auto& v : g_voices) {
    if (v.key == data) return true;
  }
  return false;
}

void Stop_Sample_Playing(void const* data) {
  if (!data) return;
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  g_voices.erase(std::remove_if(g_voices.begin(), g_voices.end(), [&](const Voice& v) { return v.key == data; }),
                 g_voices.end());
}

int Sample_Status(int handle) {
  if (handle <= 0) return 0;
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  for (const auto& v : g_voices) {
    if (v.handle == handle) return 1;
  }
  return 0;
}

void Fade_Sample(int handle, int ticks) {
  if (handle <= 0) return;
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  for (auto& v : g_voices) {
    if (v.handle == handle) {
      v.fade_ticks_total = std::max(0, ticks);
      v.fade_ticks_remaining = v.fade_ticks_total;
      v.fade_start_volume = v.volume;
      return;
    }
  }
}

void Sound_Callback() {
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  for (auto it = g_voices.begin(); it != g_voices.end();) {
    if (it->fade_ticks_remaining > 0) {
      it->fade_ticks_remaining--;
      const int denom = std::max(1, it->fade_ticks_total);
      it->volume = (it->fade_start_volume * it->fade_ticks_remaining) / denom;
      if (it->fade_ticks_remaining == 0) {
        it = g_voices.erase(it);
        continue;
      }
    }
    ++it;
  }
}

int File_Stream_Sample_Vol(char const* name, int volume, bool loop) {
  if (!name || !*name) return 0;

  const void* ptr = MixFileClass::Retrieve(name);
  if (!ptr) {
    std::string with_ext = name;
    if (with_ext.find('.') == std::string::npos) with_ext += ".AUD";
    ptr = MixFileClass::Retrieve(with_ext.c_str());
  }
  if (!ptr) return 0;

  return Play_Aud_Ptr(ptr, volume, 255, 0, loop, true);
}
