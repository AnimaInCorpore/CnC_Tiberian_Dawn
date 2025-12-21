// SDL mixer-backed playback for legacy sound effect and EVA calls.
// Ported to replace the previous no-op stubs.

#include <SDL.h>

#include "function.h"
#include "legacy/audio_stub.h"

#include <algorithm>
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
  int priority = 0;
  int volume = 0;   // 0..255
  int pan = 0;      // -32767..32767
  std::size_t pos = 0;
};

std::mutex g_cache_mutex;
std::unordered_map<const void*, DecodedAudio> g_cache;
std::vector<Voice> g_voices;
int g_next_handle = 1;

inline int ClampInt(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

static bool Read_Aud_Header(const std::uint8_t* data, std::size_t size,
                            int& sample_rate, std::uint32_t& data_size, std::uint32_t& output_size,
                            std::uint8_t& flags, AudFormat& fmt, std::size_t& data_offset) {
  if (!data || size < 16) return false;

  sample_rate = static_cast<int>(data[0] | (data[1] << 8));
  data_size = static_cast<std::uint32_t>(data[2] | (data[3] << 8) | (data[4] << 16) | (data[5] << 24));
  output_size =
      static_cast<std::uint32_t>(data[6] | (data[7] << 8) | (data[8] << 16) | (data[9] << 24));
  flags = data[10];
  fmt = static_cast<AudFormat>(data[11]);
  data_offset = 12;
  return true;
}

static bool Read_Chunk_Header(const std::uint8_t* data, std::size_t size, std::size_t offset,
                              AudChunkHeader& out, std::size_t& next) {
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
          // single signed byte delta
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
      7,     8,     9,     10,    11,    12,    13,    14,    16,    17,    19,    21,    23,
      25,    28,    31,    34,    37,    41,    45,    50,    55,    60,    66,    73,    80,
      88,    97,    107,   118,   130,   143,   157,   173,   190,   209,   230,   253,   279,
      307,   337,   371,   408,   449,   494,   544,   598,   658,   724,   796,   876,   963,
      1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,  3327,
      3660,  4026,  4428,  4871,  5358,  5894,  6484,  7132,  7845,  8630,  9493,  10442, 11487,
      12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};

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

static bool Decode_Aud_To_Device_Pcm(const void* key, DecodedAudio& decoded) {
  const auto* src = static_cast<const std::uint8_t*>(key);
  if (!src) return false;

  SDL_AudioSpec const* spec = Audio_Get_Spec();
  if (!spec || spec->freq <= 0) return false;

  int sample_rate = 0;
  std::uint32_t data_size = 0;
  std::uint32_t output_size = 0;
  std::uint8_t flags = 0;
  AudFormat format = AudFormat::WestwoodCompressed;
  std::size_t offset = 0;
  if (!Read_Aud_Header(src, 1u << 30, sample_rate, data_size, output_size, flags, format, offset)) {
    return false;
  }

  // The game predominantly uses 8-bit mono AUD. Keep behavior aligned to the device request.
  const bool want_s16 = (spec->format == AUDIO_S16SYS);
  const bool want_u8 = (spec->format == AUDIO_U8);
  if (!want_s16 && !want_u8) return false;

  decoded.sample_rate = spec->freq;
  decoded.format = spec->format;
  decoded.channels = spec->channels;

  // Decode the compressed stream into a temporary mono buffer (8-bit or 16-bit LE).
  std::vector<std::uint8_t> mono;
  mono.reserve(output_size);

  std::size_t cursor = offset;
  std::uint32_t remaining = data_size;
  while (remaining >= 8) {
    AudChunkHeader chunk{};
    std::size_t chunk_data = 0;
    if (!Read_Chunk_Header(src, cursor + 8 + remaining, cursor, chunk, chunk_data)) break;
    cursor = chunk_data;
    if (chunk.compressed_size > remaining) break;

    const std::uint8_t* chunk_input = src + cursor;
    cursor += chunk.compressed_size;
    remaining -= static_cast<std::uint32_t>(8 + chunk.compressed_size);

    if (format == AudFormat::WestwoodCompressed) {
      const std::size_t out_start = mono.size();
      mono.resize(out_start + chunk.output_size);
      Decode_Westwood_Compressed(chunk_input, chunk.compressed_size, mono.data() + out_start, chunk.output_size);
    } else if (format == AudFormat::ImaAdpcm) {
      // IMA ADPCM expands into signed 16-bit little endian.
      int current = 0;
      int index = 0;
      for (std::size_t n = 0; n < chunk.compressed_size; ++n) {
        const std::uint8_t b = chunk_input[n];
        const std::int16_t s0 = Decode_Ima_Sample(b, index, current);
        mono.push_back(static_cast<std::uint8_t>(s0 & 0xFF));
        mono.push_back(static_cast<std::uint8_t>((s0 >> 8) & 0xFF));
        if (mono.size() / 2 >= output_size / 2) break;
        const std::int16_t s1 = Decode_Ima_Sample(static_cast<std::uint8_t>(b >> 4), index, current);
        mono.push_back(static_cast<std::uint8_t>(s1 & 0xFF));
        mono.push_back(static_cast<std::uint8_t>((s1 >> 8) & 0xFF));
      }
    } else {
      break;
    }
  }

  // Convert mono buffer into device format/channels.
  if (want_u8) {
    decoded.pcm = std::move(mono);
    if (decoded.channels == 2) {
      std::vector<std::uint8_t> stereo;
      stereo.resize(decoded.pcm.size() * 2);
      for (std::size_t i = 0; i < decoded.pcm.size(); ++i) {
        stereo[i * 2 + 0] = decoded.pcm[i];
        stereo[i * 2 + 1] = decoded.pcm[i];
      }
      decoded.pcm = std::move(stereo);
    }
  } else {
    // Best-effort: treat the decoded stream as unsigned 8-bit and upconvert to signed 16.
    std::vector<std::int16_t> pcm16;
    if (format == AudFormat::ImaAdpcm) {
      const std::size_t samples = mono.size() / 2;
      pcm16.resize(samples);
      std::memcpy(pcm16.data(), mono.data(), samples * 2);
    } else {
      pcm16.resize(mono.size());
      for (std::size_t i = 0; i < mono.size(); ++i) {
        pcm16[i] = static_cast<std::int16_t>((static_cast<int>(mono[i]) - 128) << 8);
      }
    }

    if (decoded.channels == 2) {
      std::vector<std::int16_t> stereo;
      stereo.resize(pcm16.size() * 2);
      for (std::size_t i = 0; i < pcm16.size(); ++i) {
        stereo[i * 2 + 0] = pcm16[i];
        stereo[i * 2 + 1] = pcm16[i];
      }
      decoded.pcm.resize(stereo.size() * 2);
      std::memcpy(decoded.pcm.data(), stereo.data(), decoded.pcm.size());
    } else {
      decoded.pcm.resize(pcm16.size() * 2);
      std::memcpy(decoded.pcm.data(), pcm16.data(), decoded.pcm.size());
    }
  }

  return !decoded.pcm.empty();
}

static DecodedAudio* Get_Decoded(const void* key) {
  std::lock_guard<std::mutex> lock(g_cache_mutex);
  auto it = g_cache.find(key);
  if (it != g_cache.end()) return &it->second;

  DecodedAudio decoded;
  if (!Decode_Aud_To_Device_Pcm(key, decoded)) return nullptr;

  auto [inserted, ok] = g_cache.emplace(key, std::move(decoded));
  if (!ok) return nullptr;
  return &inserted->second;
}

static void Stop_Voices_For_Key(const void* key) {
  g_voices.erase(std::remove_if(g_voices.begin(), g_voices.end(),
                               [&](const Voice& v) { return v.key == key; }),
                 g_voices.end());
}

}  // namespace

extern "C" void Audio_Mix_Callback(void*, Uint8* stream, int len) {
  if (!stream || len <= 0) return;
  SDL_AudioSpec const* spec = Audio_Get_Spec();
  if (!spec) {
    std::memset(stream, 0, static_cast<std::size_t>(len));
    return;
  }

  const bool s16 = spec->format == AUDIO_S16SYS;
  const bool u8 = spec->format == AUDIO_U8;
  if (!s16 && !u8) {
    std::memset(stream, 0, static_cast<std::size_t>(len));
    return;
  }

  const int channels = spec->channels > 0 ? spec->channels : 2;

  if (u8) {
    std::memset(stream, 128, static_cast<std::size_t>(len));
  } else {
    std::memset(stream, 0, static_cast<std::size_t>(len));
  }

  if (g_voices.empty()) return;

  if (s16) {
    auto* out = reinterpret_cast<std::int16_t*>(stream);
    const int samples = len / 2;
    for (int i = 0; i < samples; ++i) {
      int mixed = out[i];
      for (auto& voice : g_voices) {
        DecodedAudio* decoded = Get_Decoded(voice.key);
        if (!decoded) continue;
        const auto* pcm = reinterpret_cast<const std::int16_t*>(decoded->pcm.data());
        const int pcm_samples = static_cast<int>(decoded->pcm.size() / 2);
        if (static_cast<int>(voice.pos / 2) >= pcm_samples) continue;

        const int v = pcm[voice.pos / 2];
        voice.pos += 2;
        const int scaled = (v * voice.volume) / 255;
        mixed = ClampInt(mixed + scaled, -32768, 32767);
      }
      out[i] = static_cast<std::int16_t>(mixed);
    }
  } else {
    auto* out = reinterpret_cast<std::uint8_t*>(stream);
    for (int i = 0; i < len; ++i) {
      int mixed = static_cast<int>(out[i]) - 128;
      for (auto& voice : g_voices) {
        DecodedAudio* decoded = Get_Decoded(voice.key);
        if (!decoded) continue;
        if (voice.pos >= decoded->pcm.size()) continue;
        const int v = static_cast<int>(decoded->pcm[voice.pos++]) - 128;
        const int scaled = (v * voice.volume) / 255;
        mixed = ClampInt(mixed + scaled, -128, 127);
      }
      out[i] = static_cast<std::uint8_t>(mixed + 128);
    }
  }

  // Drop finished voices.
  g_voices.erase(std::remove_if(g_voices.begin(), g_voices.end(),
                               [&](const Voice& v) {
                                 DecodedAudio* decoded = Get_Decoded(v.key);
                                 return !decoded || v.pos >= decoded->pcm.size();
                               }),
                 g_voices.end());
}

int Play_Sample(void const* data, int priority, int volume, int pan) {
  if (!data) return -1;

  SDL_AudioDeviceID device = Audio_Get_Device();
  if (device == 0) return -1;

  SDL_LockAudioDevice(device);
  DecodedAudio* decoded = Get_Decoded(data);
  if (!decoded) {
    SDL_UnlockAudioDevice(device);
    return -1;
  }

  Voice voice{};
  voice.key = data;
  voice.priority = priority;
  voice.volume = ClampInt(volume, 0, 255);
  voice.pan = ClampInt(pan, -32767, 32767);
  voice.pos = 0;

  constexpr std::size_t kMaxVoices = 16;
  if (g_voices.size() >= kMaxVoices) {
    auto it = std::min_element(g_voices.begin(), g_voices.end(),
                               [](const Voice& a, const Voice& b) { return a.priority < b.priority; });
    if (it != g_voices.end() && it->priority <= voice.priority) {
      *it = voice;
    } else {
      SDL_UnlockAudioDevice(device);
      return -1;
    }
  } else {
    g_voices.push_back(voice);
  }

  const int handle = g_next_handle++;
  SDL_UnlockAudioDevice(device);
  return handle;
}

int Play_Sample(void const* data, int priority, int volume) { return Play_Sample(data, priority, volume, 0); }

bool Is_Sample_Playing(void const* data) {
  if (!data) return false;
  SDL_AudioDeviceID device = Audio_Get_Device();
  if (device == 0) return false;
  SDL_LockAudioDevice(device);
  const bool playing = std::any_of(g_voices.begin(), g_voices.end(), [&](const Voice& v) { return v.key == data; });
  SDL_UnlockAudioDevice(device);
  return playing;
}

void Stop_Sample_Playing(void const* data) {
  if (!data) return;
  SDL_AudioDeviceID device = Audio_Get_Device();
  if (device == 0) return;
  SDL_LockAudioDevice(device);
  Stop_Voices_For_Key(data);
  SDL_UnlockAudioDevice(device);
}

void Sound_Callback() {}
void Fade_Sample(int, int) {}

int File_Stream_Sample_Vol(char const*, int, bool) { return -1; }

void Stop_Sample(int) {}
int Sample_Status(int) { return 0; }

