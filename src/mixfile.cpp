#include "legacy/mixfile.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <strings.h>
#include <vector>

#include "legacy/ccfile.h"
#include "legacy/function.h"
#include "legacy/rawfile.h"

namespace {

unsigned long Crc_For_Name(char const* filename) {
  if (!filename) return 0;
  std::string upper = filename;
  std::transform(upper.begin(), upper.end(), upper.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return Compute_Name_CRC(upper.data());
}

}  // namespace

MixFileClass* MixFileClass::First = nullptr;

MixFileClass::MixFileClass(char const* filename)
  : Filename(nullptr), Count(0), DataSize(0), Buffer(nullptr), Data(nullptr) {
  if (!filename) return;

  CCFileClass file(filename);
  Filename = ::strdup(filename);
  FileHeader header{};

  if (!file.Open()) {
    return;
  }

  file.Read(&header, sizeof(header));
  Count = header.count;
  DataSize = header.size;

  if (Count > 0) {
    Buffer = new SubBlock[Count];
    if (Buffer) {
      file.Read(Buffer, Count * static_cast<long>(sizeof(SubBlock)));
      std::sort(Buffer, Buffer + Count,
                [](SubBlock const& a, SubBlock const& b) { return a.CRC < b.CRC; });
    }
  }
  file.Close();

  Zap();
  if (!First) {
    First = this;
  } else {
    Add_Tail(*First);
  }
}

MixFileClass::~MixFileClass() {
  if (Filename) {
    std::free(const_cast<char*>(Filename));
    Filename = nullptr;
  }
  delete[] Buffer;
  Buffer = nullptr;
  delete[] static_cast<unsigned char*>(Data);
  Data = nullptr;

  if (this == First) {
    First = static_cast<MixFileClass*>(Get_Next());
  }
  Remove();
  Zap();
}

bool MixFileClass::Free(char const* filename) {
  MixFileClass* ptr = Finder(filename);
  if (ptr) {
    ptr->Free();
    return true;
  }
  return false;
}

void MixFileClass::Free_All(void) {
  while (First) {
    delete First;
  }
}

void MixFileClass::Free(void) {
  delete[] static_cast<unsigned char*>(Data);
  Data = nullptr;
}

bool MixFileClass::Cache(char const* filename) {
  MixFileClass* mixer = Finder(filename);
  if (mixer) {
    return mixer->Cache();
  }
  return false;
}

bool MixFileClass::Cache(void) {
  if (Data || !Filename) return true;
  if (DataSize == 0) return false;

  auto* buffer = new unsigned char[static_cast<std::size_t>(DataSize)];
  if (!buffer) return false;

  CCFileClass file(Filename);
  if (!file.Open(READ)) {
    delete[] buffer;
    return false;
  }
  file.Seek(sizeof(SubBlock) * Count + sizeof(FileHeader), SEEK_SET);
  const long read = file.Read(buffer, static_cast<long>(DataSize));
  file.Close();

  if (read != static_cast<long>(DataSize)) {
    delete[] buffer;
    return false;
  }

  Data = buffer;
  return true;
}

MixFileClass* MixFileClass::Finder(char const* filename) {
  MixFileClass* ptr = First;
  while (ptr) {
    if (ptr->Filename && filename) {
      const std::size_t len = std::strlen(filename);
      const std::size_t stored_len = std::strlen(ptr->Filename);
      const char* tail = ptr->Filename + (stored_len > len ? stored_len - len : 0);
      if (strcasecmp(tail, filename) == 0) {
        return ptr;
      }
    }
    ptr = static_cast<MixFileClass*>(ptr->Get_Next());
  }
  return nullptr;
}

bool MixFileClass::Offset(char const* filename, void** realptr, MixFileClass** mixfile, long* offset,
                          long* size) {
  if (!filename) return false;

  const std::uint32_t crc = static_cast<std::uint32_t>(Crc_For_Name(filename));

  MixFileClass* ptr = First;
  while (ptr) {
    const SubBlock key{crc, 0, 0};
    const SubBlock* block =
        std::lower_bound(ptr->Buffer, ptr->Buffer + ptr->Count, key,
                         [](SubBlock const& a, SubBlock const& b) { return a.CRC < b.CRC; });

    if (block != ptr->Buffer + ptr->Count && block->CRC == crc) {
      if (mixfile) *mixfile = ptr;
      if (size) *size = static_cast<long>(block->Size);
      if (offset) {
        *offset =
            static_cast<long>(block->Offset + sizeof(SubBlock) * ptr->Count + sizeof(FileHeader));
      }
      if (realptr) {
        if (ptr->Data || ptr->Cache()) {
          *realptr = static_cast<unsigned char*>(ptr->Data) + block->Offset;
        } else {
          *realptr = nullptr;
        }
      }
      return true;
    }

    ptr = static_cast<MixFileClass*>(ptr->Get_Next());
  }
  return false;
}

void const* MixFileClass::Retrieve(char const* filename) {
  void* ptr = nullptr;
  MixFileClass::Offset(filename, &ptr);
  return ptr;
}
