#include "legacy/function.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>

namespace {

int MemICmp(const void* a, const void* b, int length) {
  if (!a || !b || length <= 0) return 0;
  auto const* lhs = static_cast<unsigned char const*>(a);
  auto const* rhs = static_cast<unsigned char const*>(b);
  for (int i = 0; i < length; ++i) {
    const unsigned char cl = static_cast<unsigned char>(std::toupper(lhs[i]));
    const unsigned char cr = static_cast<unsigned char>(std::toupper(rhs[i]));
    if (cl != cr) {
      return static_cast<int>(cl) - static_cast<int>(cr);
    }
  }
  return 0;
}

void StrUpr(char* buffer) {
  if (!buffer) return;
  for (; *buffer; ++buffer) {
    *buffer = static_cast<char>(std::toupper(static_cast<unsigned char>(*buffer)));
  }
}

}  // namespace

bool Read_Private_Config_Struct(char* profile, NewConfigType* config) {
  if (!config) return true;

  config->DigitCard = WWGetPrivateProfileHex("Sound", "Card", profile);
  config->IRQ = WWGetPrivateProfileInt("Sound", "IRQ", 0, profile);
  config->DMA = WWGetPrivateProfileInt("Sound", "DMA", 0, profile);
  config->Port = WWGetPrivateProfileHex("Sound", "Port", profile);
  config->BitsPerSample = WWGetPrivateProfileInt("Sound", "BitsPerSample", 0, profile);
  config->Channels = WWGetPrivateProfileInt("Sound", "Channels", 0, profile);
  config->Reverse = WWGetPrivateProfileInt("Sound", "Reverse", 0, profile);
  config->Speed = WWGetPrivateProfileInt("Sound", "Speed", 0, profile);
  WWGetPrivateProfileString("Language", "Language", nullptr, config->Language, 3, profile);

  return ((config->DigitCard == 0) && (config->IRQ == 0) && (config->DMA == 0));
}

unsigned WWGetPrivateProfileHex(char const* section, char const* entry, char* profile) {
  char buffer[50];
  char* ptr = WWGetPrivateProfileString(section, entry, nullptr, buffer, sizeof(buffer), profile);
  if (!ptr) return 0;
  unsigned value = 0;
  std::sscanf(buffer, "%x", &value);
  return value;
}

int WWGetPrivateProfileInt(char const* section, char const* entry, int def, char* profile) {
  char buffer[50];
  if (!WWGetPrivateProfileString(section, entry, nullptr, buffer, sizeof(buffer), profile)) {
    return def;
  }
  return std::atoi(buffer);
}

bool WWWritePrivateProfileInt(char const* section, char const* entry, int value, char* profile) {
  char buffer[250];

  if (!profile || !section) {
    return true;
  }

  std::snprintf(buffer, sizeof(buffer), "%d", value);
  return WWWritePrivateProfileString(section, entry, buffer, profile);
}

char* WWGetPrivateProfileString(char const* section,
                                char const* entry,
                                char const* def,
                                char* retbuffer,
                                int retlen,
                                char* profile) {
  char* workptr = nullptr;
  char* altworkptr = nullptr;
  char sec[50];
  char* retval = nullptr;
  char* next = nullptr;
  char c = 0;
  char c2 = 0;
  int len = 0;
  int entrylen = 0;
  char* orig_retbuf = nullptr;

  if (retbuffer) {
    if (def) {
      std::strncpy(retbuffer, def, retlen);
    } else if (retlen > 0) {
      retbuffer[0] = '\0';
    }
    if (retlen > 0) {
      retbuffer[retlen - 1] = '\0';
    }
    orig_retbuf = retbuffer;
  }

  if (!profile || !section) {
    return retbuffer;
  }

  std::snprintf(sec, sizeof(sec), "[%s]", section);
  StrUpr(sec);
  len = static_cast<int>(std::strlen(sec));

  retval = profile;
  workptr = profile;
  for (;;) {
    workptr = std::strchr(workptr, '[');
    if (!workptr) {
      return nullptr;
    }

    if (workptr == profile) {
      c = '\n';
    } else {
      c = *(workptr - 1);
    }

    if (MemICmp(workptr, sec, len) == 0 && (c == '\n')) {
      workptr += len;
      while (std::isspace(static_cast<unsigned char>(*workptr))) {
        workptr++;
      }

      if (workptr - profile > 4) {
        if (*(workptr - 1) == '\n' && *(workptr - 3) == '\n') workptr -= 2;
      }

      next = std::strchr(workptr, '[');
      for (;;) {
        if (next) {
          c = *(next - 1);
          if (c == '\n') {
            if (*(next - 1) == '\n' && *(next - 3) == '\n') {
              next -= 2;
            }
            break;
          }
          next = std::strchr(next + 1, '[');
        } else {
          next = workptr + std::strlen(workptr) - 1;
          break;
        }
      }

      if (entry) {
        retval = workptr;
        entrylen = static_cast<int>(std::strlen(entry));

        for (;;) {
          workptr = std::strchr(workptr, *entry);
          if (!workptr || workptr >= next) {
            return nullptr;
          }

          c = *(workptr - 1);
          c2 = *(workptr + entrylen);

          if (MemICmp(workptr, entry, entrylen) == 0 && (c == '\n') && (c2 == '=' || std::isspace(static_cast<unsigned char>(c2)))) {
            retval = workptr;
            workptr += entrylen;
            workptr = std::strchr(workptr, '=');

            if (workptr) {
              altworkptr = std::strchr(workptr, '\r');
            }

            if (workptr == nullptr || altworkptr < workptr) {
              return retval;
            }

            workptr++;
            while (std::isspace(static_cast<unsigned char>(*workptr))) {
              if (workptr >= altworkptr) return retval;
              workptr++;
            }

            len = static_cast<int>(altworkptr - workptr);
            if (len > retlen - 1) {
              len = retlen - 1;
            }

            if (retbuffer && retlen > 0) {
              std::memcpy(retbuffer, workptr, len);
              *(retbuffer + len) = '\0';
              strtrim(retbuffer);
            }
            return retval;
          }

          workptr++;
        }
      } else {
        retval = workptr;
        if (retbuffer) {
          while (workptr && workptr < next) {
            altworkptr = std::strchr(workptr, '=');
            if (altworkptr && altworkptr < next) {
              int length = static_cast<int>(altworkptr - workptr);
              if (retbuffer - orig_retbuf + length + 3 < retlen) {
                std::memcpy(retbuffer, workptr, length);
                *(retbuffer + length) = '\0';
                strtrim(retbuffer);
                retbuffer += std::strlen(retbuffer) + 1;
              } else {
                break;
              }
              workptr = std::strchr(altworkptr, '\n');
              if (!workptr) break;
              workptr++;
            } else {
              break;
            }
          }
          *retbuffer++ = '\0';
          *retbuffer++ = '\0';
        }
        break;
      }
    } else {
      workptr++;
    }
  }

  return retval;
}

bool WWWritePrivateProfileString(char const* section, char const* entry, char const* string, char* profile) {
  char buffer[250];
  char* offset = nullptr;
  char* next = nullptr;
  char c = 0;

  if (!profile || !section) {
    return true;
  }

  offset = WWGetPrivateProfileString(section, nullptr, nullptr, nullptr, 0, profile);

  if (!offset && entry) {
    std::snprintf(buffer, sizeof(buffer), "\r\n[%s]\r\n", section);
    std::strcat(profile, buffer);
  }

  if (offset && !entry) {
    next = std::strchr(offset, '[');
    for (;;) {
      if (next) {
        c = *(next - 1);
        if (c == '\n') {
          if (*(next - 1) == '\n' && *(next - 3) == '\n') {
            next -= 2;
          }
          break;
        }
        next = std::strchr(next + 1, '[');
      } else {
        next = offset + std::strlen(offset);
        break;
      }
    }

    std::strcpy(offset, next);
    return true;
  }

  offset = WWGetPrivateProfileString(section, entry, nullptr, nullptr, 0, profile);

  if (offset) {
    const int eol = static_cast<int>(std::strcspn(offset, "\n"));
    if (eol) {
      std::strcpy(offset, offset + eol + 1);
    }
  } else {
    offset = WWGetPrivateProfileString(section, nullptr, nullptr, nullptr, 0, profile);
  }

  if (entry && string) {
    std::snprintf(buffer, sizeof(buffer), "%s=%s\r\n", entry, string);

    std::memmove(offset + std::strlen(buffer), offset, std::strlen(offset) + 1);
    std::memcpy(offset, buffer, std::strlen(buffer));
  }

  return true;
}

