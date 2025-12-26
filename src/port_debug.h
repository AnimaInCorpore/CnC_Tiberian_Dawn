#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>

inline bool TD_Verbose_Startup_Enabled() {
  const char* value = std::getenv("TD_VERBOSE");
  if (!value || !*value) return false;
  return std::strcmp(value, "0") != 0 && std::strcmp(value, "false") != 0 &&
         std::strcmp(value, "FALSE") != 0;
}

inline void TD_Debugf(const char* fmt, ...) {
  if (!TD_Verbose_Startup_Enabled()) return;

  using clock = std::chrono::steady_clock;
  static const auto start = clock::now();
  const auto now = clock::now();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

  std::fprintf(stderr, "[td %8lldms] ", static_cast<long long>(ms));

  va_list args;
  va_start(args, fmt);
  std::vfprintf(stderr, fmt ? fmt : "", args);
  va_end(args);

  if (fmt) {
    const std::size_t len = std::strlen(fmt);
    if (len == 0 || fmt[len - 1] != '\n') {
      std::fputc('\n', stderr);
    }
  } else {
    std::fputc('\n', stderr);
  }
}

