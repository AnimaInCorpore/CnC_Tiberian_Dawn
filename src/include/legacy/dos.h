#pragma once

#include <cstdint>

// Minimal DOS compatibility layer so the legacy code can compile on non-DOS
// toolchains. The functions are only declared here; the eventual platform
// shims will supply working implementations.

struct find_t {
  char name[13];
  std::uint8_t attrib;
  std::uint16_t wr_time;
  std::uint16_t wr_date;
  std::uint32_t size;
  char reserved[21];
};

constexpr std::uint16_t _A_NORMAL = 0x00;
constexpr std::uint16_t _A_RDONLY = 0x01;
constexpr std::uint16_t _A_HIDDEN = 0x02;
constexpr std::uint16_t _A_SYSTEM = 0x04;
constexpr std::uint16_t _A_SUBDIR = 0x10;
constexpr std::uint16_t _A_ARCH = 0x20;

int _dos_open(char const* path, int mode, int* handle);
int _dos_creat(char const* path, unsigned attributes, int* handle);
int _dos_close(int handle);
int _dos_read(int handle, void* buffer, unsigned count, unsigned* bytes_read);
int _dos_write(int handle, void const* buffer, unsigned count, unsigned* bytes_written);
int _dos_findfirst(char const* pattern, unsigned attributes, find_t* info);
int _dos_findnext(find_t* info);
void _dos_getdrive(unsigned* drive);
void _dos_setdrive(unsigned drive, unsigned* lastdrive);
int _dos_getdiskfree(unsigned drive, void* diskfree);
