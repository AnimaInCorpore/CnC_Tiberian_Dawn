#pragma once

#include <cstddef>
#include <cstdint>

#include "compat.h"

class FileClass {
 public:
  virtual ~FileClass() = default;

  virtual char const* File_Name() const = 0;
  virtual char const* Set_Name(char const* filename) = 0;
  virtual int Create() = 0;
  virtual int Delete() = 0;
  virtual int Is_Available(int forced = false) = 0;
  virtual int Is_Open() const = 0;
  virtual int Open(char const* filename, int rights = READ) = 0;
  virtual int Open(int rights = READ) = 0;
  virtual long Read(void* buffer, long size) = 0;
  virtual long Seek(long pos, int dir = SEEK_CUR) = 0;
  virtual long Size() = 0;
  virtual long Write(void const* buffer, long size) = 0;
  virtual void Close() = 0;

  operator char const*() const { return File_Name(); }
};

namespace cnc {

constexpr int kFileAccessRead = READ;
constexpr int kFileAccessWrite = WRITE;

using FileClass = ::FileClass;

}  // namespace cnc
