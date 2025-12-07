#include "legacy/rawfile.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "legacy/wwlib32.h"

namespace {

int Open_Flags(int rights) {
  const bool can_read = (rights & READ) != 0;
  const bool can_write = (rights & WRITE) != 0;

  if (can_read && can_write) return O_RDWR;
  if (can_write) return O_WRONLY;
  return O_RDONLY;
}

}  // namespace

RawFileClass::RawFileClass(char const* filename)
    : Rights(READ), Handle(-1), Filename(nullptr), Allocated(0) {
  Set_Name(filename);
}

RawFileClass::RawFileClass()
    : Rights(READ), Handle(-1), Filename(nullptr), Allocated(0) {}

RawFileClass::RawFileClass(RawFileClass const& file)
    : Rights(file.Rights), Handle(-1), Filename(nullptr), Allocated(0) {
  Set_Name(file.File_Name());
}

RawFileClass& RawFileClass::operator=(RawFileClass const& file) {
  if (this != &file) {
    Rights = file.Rights;
    Close();
    Set_Name(file.File_Name());
  }
  return *this;
}

RawFileClass::~RawFileClass() { Close(); }

char const* RawFileClass::File_Name() const { return Filename; }

char const* RawFileClass::Set_Name(char const* filename) {
  if (Allocated && Filename) {
    delete[] Filename;
  }
  Filename = nullptr;
  Allocated = 0;

  if (filename && *filename) {
    const std::string copy = filename;
    char* buffer = new char[copy.size() + 1];
    std::memcpy(buffer, copy.c_str(), copy.size() + 1);
    Filename = buffer;
    Allocated = 1;
  }
  return Filename;
}

int RawFileClass::Create() {
  if (!Filename) return 0;

  Handle = ::open(Filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
  return (Handle >= 0) ? 1 : 0;
}

int RawFileClass::Delete() {
  if (!Filename) return 0;
  Close();
  return (::unlink(Filename) == 0) ? 1 : 0;
}

int RawFileClass::Is_Available(int /*forced*/) {
  if (!Filename) return 0;
  struct stat info {};
  return (::stat(Filename, &info) == 0) ? 1 : 0;
}

int RawFileClass::Is_Open() const { return Handle >= 0; }

int RawFileClass::Open(char const* filename, int rights) {
  Set_Name(filename);
  return Open(rights);
}

int RawFileClass::Open(int rights) {
  if (!Filename) return 0;
  Rights = rights;
  Handle = ::open(Filename, Open_Flags(rights));
  return (Handle >= 0) ? 1 : 0;
}

long RawFileClass::Read(void* buffer, long size) {
  if (!Is_Open() || !buffer || size <= 0) return 0;
  const ssize_t read_bytes = ::read(Handle, buffer, static_cast<size_t>(size));
  return (read_bytes < 0) ? 0 : static_cast<long>(read_bytes);
}

long RawFileClass::Seek(long pos, int dir) {
  if (!Is_Open()) return 0;
  const off_t new_pos = ::lseek(Handle, pos, dir);
  return (new_pos < 0) ? 0 : static_cast<long>(new_pos);
}

long RawFileClass::Size() {
  if (!Filename) return 0;
  struct stat info {};
  if (::stat(Filename, &info) != 0) return 0;
  return static_cast<long>(info.st_size);
}

long RawFileClass::Write(void const* buffer, long size) {
  if (!Is_Open() || !buffer || size <= 0) return 0;
  const ssize_t written =
      ::write(Handle, buffer, static_cast<size_t>(size));
  return (written < 0) ? 0 : static_cast<long>(written);
}

void RawFileClass::Close() {
  if (Handle >= 0) {
    ::close(Handle);
    Handle = -1;
  }
}

void RawFileClass::Error(int /*error*/, int /*canretry*/, char const* /*filename*/) {}
