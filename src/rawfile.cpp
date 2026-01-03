#include "legacy/rawfile.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <new>
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
#include "legacy/msgbox.h"
#include "legacy/wwlib32.h"

namespace {
bool Can_Show_Graphic_Error() {
  return FontPtr != nullptr && SeenBuff.Get_Width() > 0 && SeenBuff.Get_Height() > 0;
}

bool Commit_File_Handle(int handle) {
  if (handle < 0) {
    return false;
  }
#if defined(_WIN32)
  return ::_commit(handle) == 0;
#else
  return ::fsync(handle) == 0;
#endif
}

KeyNumType Wait_For_Any_Key() {
  Keyboard::Clear();
  for (;;) {
    const KeyNumType input = Keyboard::Get();
    if (input != KN_NONE) {
      return input;
    }
    Call_Back();
  }
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
    char* buffer = new (std::nothrow) char[copy.size() + 1];
    if (!buffer) {
      Error(ENOMEM, false, filename);
      return nullptr;
    }
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

    const int open_errno = Hard_Error_Occured ? static_cast<int>(Hard_Error_Occured)
                                              : (errno ? errno : EIO);
    const int canretry = (Hard_Error_Occured || open_errno == ENOENT) ? 1 : 0;
    Error(open_errno, canretry, Filename);
    if (!canretry) {
      return 0;
    }
  }
}

long RawFileClass::Read(void* buffer, long size) {
  if (!buffer || size <= 0) return 0;

  int opened = false;
  if (!Is_Open()) {
    if (!Open(READ)) {
      return 0;
    }
    opened = true;
  }

  long bytesread = 0;
  unsigned char* out = static_cast<unsigned char*>(buffer);
  while (size > 0) {
    const long desired = (size < Transfer_Block_Size()) ? size : Transfer_Block_Size();
    const ssize_t read_bytes = ::read(Handle, out, static_cast<size_t>(desired));
    if (read_bytes < 0) {
      const int read_errno = errno ? errno : EIO;
      Error(read_errno, 0, Filename);
      break;
    }
    if (read_bytes == 0) {
      break;
    }
    out += read_bytes;
    bytesread += static_cast<long>(read_bytes);
    size -= static_cast<long>(read_bytes);
    if (read_bytes != desired) {
      break;
    }
  }

  if (opened) {
    Close();
  }
  return bytesread;
}

long RawFileClass::Seek(long pos, int dir) {
  if (!Is_Open()) {
    Error(EBADF, false, Filename);
    return 0;
  }

  const off_t new_pos = ::lseek(Handle, pos, dir);
  if (new_pos < 0) {
    const int seek_errno = errno ? errno : EIO;
    Error(seek_errno, false, Filename);
    return 0;
  }
  return static_cast<long>(new_pos);
}

long RawFileClass::Size() {
  if (!Filename) return 0;
  struct stat info {};
  if (::stat(Filename, &info) != 0) return 0;
  return static_cast<long>(info.st_size);
}

long RawFileClass::Write(void const* buffer, long size) {
  if (!buffer || size <= 0) return 0;

  int opened = false;
  if (!Is_Open()) {
    if (!Open(WRITE)) {
      return 0;
    }
    opened = true;
  }

  long byteswritten = 0;
  const unsigned char* in = static_cast<const unsigned char*>(buffer);
  while (size > 0) {
    const long desired = (size < Transfer_Block_Size()) ? size : Transfer_Block_Size();
    const ssize_t written = ::write(Handle, in, static_cast<size_t>(desired));
    if (written < 0) {
      const int write_errno = errno ? errno : EIO;
      Error(write_errno, false, Filename);
      break;
    }
    if (written == 0) {
      Error(EIO, false, Filename);
      break;
    }
    in += written;
    byteswritten += static_cast<long>(written);
    size -= static_cast<long>(written);
    if (written != desired) {
      Error(ENOSPC, false, Filename);
      break;
    }
  }

  if (opened) {
    Close();
  }
  return byteswritten;
}

void RawFileClass::Close() {
  if (Handle >= 0) {
    if ((Rights & WRITE) != 0) {
      if (!Commit_File_Handle(Handle)) {
        const int commit_errno = errno ? errno : EIO;
        Error(commit_errno, false, Filename);
      }
    }
    if (::close(Handle) != 0) {
      const int close_errno = errno ? errno : EIO;
      Handle = -1;
      Error(close_errno, false, Filename);
    }
    Handle = -1;
  }
}

void RawFileClass::Error(int error, int canretry, char const* filename) {
  std::string message;
#ifdef GERMAN
  message = "DATEIFEHLER";
#else
#ifdef FRENCH
  message = "ERREUR DE FICHIER";
#else
  message = "FILE ERROR";
#endif
#endif
  if (filename && *filename) {
    message.push_back('(');
    message.append(filename);
    message.push_back(')');
  }
  message.append(": ");
  message.append(std::strerror(error));
  message.append(".\n");

  if (canretry) {
#ifdef GERMAN
    message.append("Beliebige Taste dr\x81cken f\x81r erneuten Versuch.\n<ESC> dr\x81cken, um das Programm zu verlassen.");
#else
#ifdef FRENCH
    message.append("Appuyez sur une touche pour recommencer.\nAppuyez sur Echap pour quitter le programme.");
#else
    message.append("Press any key to retry.\nPress <ESC> to exit program.");
#endif
#endif
  } else {
#ifdef GERMAN
    message.append("Beliebige Taste dr\x81cken, um das Programm zu verlassen.");
#else
#ifdef FRENCH
    message.append("Appuyez sur une touche pour quitter le programme.");
#else
    message.append("Press any key to exit program.");
#endif
#endif
  }

  if (Can_Show_Graphic_Error()) {
    const char* button1 = canretry ? "Retry" : "OK";
    const int choice = CCMessageBox().Process(message.c_str(), button1, "Exit", nullptr, true);
    if (choice == 0 && canretry) {
      return;
    }
  } else {
    std::fprintf(stderr, "%s\n", message.c_str());
    const KeyNumType input = Wait_For_Any_Key();
    if (input != KN_ESC && canretry) {
      return;
    }
  }

  Prog_End();
  std::exit(EXIT_FAILURE);
}
