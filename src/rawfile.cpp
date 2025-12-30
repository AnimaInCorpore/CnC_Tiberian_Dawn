#include "legacy/rawfile.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#if defined(_WIN32)
#include <io.h>
#include <BaseTsd.h>
#include <stdlib.h>
#else
#include <unistd.h>
#endif

#if defined(_WIN32)
#define ssize_t SSIZE_T
#define unlink _unlink
#define open _open
#define read _read
#define write _write
#define close _close
#endif

#include "legacy/error.h"
#include "legacy/function.h"
#include "legacy/wwlib32.h"

namespace {
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

int RawFileClass::Is_Available(int forced) {
  if (!Filename) return 0;

  if (Is_Open()) {
    return 1;
  }

  if (forced) {
    if (!Open(READ)) {
      return 0;
    }
    Close();
    return 1;
  }

  struct stat info {};
  return (::stat(Filename, &info) == 0) ? 1 : 0;
}

int RawFileClass::Is_Open() const { return Handle >= 0; }

int RawFileClass::Open(char const* filename, int rights) {
  Set_Name(filename);
  return Open(rights);
}

int RawFileClass::Open(int rights) {
  Close();

  if (!Filename) {
    Error(ENOENT, false, nullptr);
    return 0;
  }

  Rights = rights;

  for (;;) {
    errno = 0;

    switch (rights) {
      case READ:
        Handle = ::open(Filename, O_RDONLY);
        break;
      case WRITE:
        Handle = ::open(Filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        break;
      case READ | WRITE:
        Handle = ::open(Filename, O_CREAT | O_RDWR, 0666);
        break;
      default:
        errno = EINVAL;
        Handle = -1;
        break;
    }

    if (Handle >= 0) {
      return 1;
    }

    const int open_errno = errno ? errno : EIO;
    const bool allow_retry = (rights == READ) && (open_errno == ENOENT);
    Error(open_errno, allow_retry ? 1 : 0, Filename);
    if (!allow_retry) {
      return 0;
    }
  }
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

void RawFileClass::Error(int error, int canretry, char const* filename) {
  std::string message = "FILE ERROR";
  if (filename && *filename) {
    message.push_back('(');
    message.append(filename);
    message.push_back(')');
  }
  message.append(": ");
  message.append(std::strerror(error));
  message.append(". ");

  if (canretry) {
    message.append("Press any key to retry. Press <ESC> to exit program.");
    std::printf("%s\n", message.c_str());
    const int key = Get_Key();
    if (key == KN_ESC) {
      Print_Error_End_Exit(const_cast<char*>(message.c_str()));
    }
    return;
  }

  message.append("Press any key to exit program.");
  Print_Error_End_Exit(const_cast<char*>(message.c_str()));
}
