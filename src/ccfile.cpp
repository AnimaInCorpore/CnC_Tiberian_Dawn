#include "legacy/ccfile.h"

#include <algorithm>
#include <cstring>

#include "legacy/mixfile.h"

CCFileClass::CCFileClass(char const* filename)
    : CDFileClass(filename), FromDisk(false), Pointer(nullptr), Start(0), Position(0), Length(0) {}

CCFileClass::CCFileClass()
    : CDFileClass(), FromDisk(false), Pointer(nullptr), Start(0), Position(0), Length(0) {}

int CCFileClass::Is_Open() const {
  if (Pointer) return true;
  return CDFileClass::Is_Open();
}

int CCFileClass::Is_Available(int forced) {
  MixFileClass* mix = nullptr;
  long offset = 0;
  long size = 0;
  if (MixFileClass::Offset(File_Name(), nullptr, &mix, &offset, &size)) {
    return 1;
  }
  return CDFileClass::Is_Available(forced);
}

int CCFileClass::Open(int rights) {
  if ((rights & WRITE) != 0) {
    return CDFileClass::Open(rights);
  }

  MixFileClass* mix = nullptr;
  long offset = 0;
  long size = 0;

  if (MixFileClass::Offset(File_Name(), nullptr, &mix, &offset, &size) && mix) {
    FromDisk = true;
    Pointer = nullptr;
    Start = offset;
    Length = size;
    Position = 0;

    // Open the parent mixfile and seek to the embedded offset.
    CDFileClass::Set_Name(mix->Filename);
    if (!CDFileClass::Open(READ)) {
      Pointer = nullptr;
      return 0;
    }
    CDFileClass::Seek(Start, SEEK_SET);
    return 1;
  }

  FromDisk = false;
  Pointer = nullptr;
  Start = 0;
  Position = 0;
  Length = 0;
  return CDFileClass::Open(rights);
}

long CCFileClass::Read(void* buffer, long size) {
  if (!buffer || size <= 0) return 0;

  if (Pointer) {
    const long clamped = std::min(size, Length - Position);
    if (clamped <= 0) return 0;
    std::memcpy(buffer, static_cast<unsigned char*>(Pointer) + Position, static_cast<size_t>(clamped));
    Position += clamped;
    return clamped;
  }

  if (FromDisk) {
    const long clamped = std::min(size, Length - Position);
    if (clamped <= 0) return 0;
    CDFileClass::Seek(Start + Position, SEEK_SET);
    const long read = CDFileClass::Read(buffer, clamped);
    Position += read;
    return read;
  }

  return CDFileClass::Read(buffer, size);
}

long CCFileClass::Seek(long pos, int dir) {
  if (Pointer || FromDisk) {
    long target = Position;
    switch (dir) {
      case SEEK_SET:
        target = pos;
        break;
      case SEEK_CUR:
        target = Position + pos;
        break;
      case SEEK_END:
        target = Length + pos;
        break;
      default:
        break;
    }
    target = std::clamp(target, 0L, Length);
    Position = target;
    return Position;
  }
  return CDFileClass::Seek(pos, dir);
}

long CCFileClass::Size(void) {
  if (Pointer || FromDisk) {
    return Length;
  }
  return CDFileClass::Size();
}

long CCFileClass::Write(void const* /*buffer*/, long /*size*/) { return 0; }

void CCFileClass::Close(void) {
  Pointer = nullptr;
  FromDisk = false;
  Start = 0;
  Position = 0;
  Length = 0;
  CDFileClass::Close();
}

void CCFileClass::Error(int error, int canretry, char const* filename) {
  CDFileClass::Error(error, canretry, filename);
}
