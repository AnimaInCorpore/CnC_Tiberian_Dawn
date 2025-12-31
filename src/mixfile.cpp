#include "legacy/mixfile.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <cctype>

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

  Load_Xcc_Name_Table();
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

  MixFileClass* ptr = First;
  while (ptr) {
    const std::uint32_t crc = ptr->Resolve_Crc_For_Name(filename);
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

std::size_t MixFileClass::Size_For_Pointer(void const* ptr) {
  if (!ptr) return 0;
  const auto* needle = static_cast<const unsigned char*>(ptr);

  MixFileClass* mix = First;
  while (mix) {
    if (mix->Data && mix->DataSize > 0 && mix->Buffer && mix->Count > 0) {
      const auto* base = static_cast<const unsigned char*>(mix->Data);
      const auto* end = base + static_cast<std::size_t>(mix->DataSize);
      if (needle >= base && needle < end) {
        const std::size_t rel = static_cast<std::size_t>(needle - base);
        for (int i = 0; i < mix->Count; ++i) {
          const SubBlock& block = mix->Buffer[i];
          const std::size_t off = static_cast<std::size_t>(block.Offset);
          const std::size_t size = static_cast<std::size_t>(block.Size);
          if (rel >= off && rel < off + size) {
            return size - (rel - off);
          }
        }
        return static_cast<std::size_t>(end - needle);
      }
    }
    mix = static_cast<MixFileClass*>(mix->Get_Next());
  }
  return 0;
}

std::string MixFileClass::Upper_Name(const char* name) {
  if (!name) return {};
  std::string upper = name;
  std::transform(upper.begin(), upper.end(), upper.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return upper;
}

std::uint32_t MixFileClass::Resolve_Crc_For_Name(char const* filename) const {
  if (!filename) return 0;
  const std::string upper = Upper_Name(filename);
  auto it = NameToCrc.find(upper);
  if (it != NameToCrc.end()) {
    return it->second;
  }
  return static_cast<std::uint32_t>(Crc_For_Name(filename));
}

void MixFileClass::Load_Xcc_Name_Table() {
  if (!Filename || Count <= 0 || !Buffer) return;

  const char kMarker[] = "XCC by Olaf van der Spek";

  CCFileClass file(Filename);
  if (!file.Open(READ)) {
    return;
  }

  // Read the original subblock order from disk. We sort Buffer for fast CRC lookups, but the optional
  // XCC name table (when present) lists names in the original on-disk subblock order.
  std::vector<SubBlock> original_blocks;
  original_blocks.resize(static_cast<std::size_t>(Count));
  file.Seek(sizeof(FileHeader), SEEK_SET);
  const long blocks_read = file.Read(original_blocks.data(), static_cast<long>(original_blocks.size() * sizeof(SubBlock)));
  if (blocks_read != static_cast<long>(original_blocks.size() * sizeof(SubBlock))) {
    file.Close();
    return;
  }

  const long table_bytes =
      static_cast<long>(sizeof(SubBlock)) * Count + static_cast<long>(sizeof(FileHeader));
  for (int i = 0; i < Count; ++i) {
    const SubBlock& block = Buffer[i];
    if (block.Size > 4096) continue;

    std::vector<unsigned char> blob(block.Size);
    file.Seek(table_bytes + static_cast<long>(block.Offset), SEEK_SET);
    const long read = file.Read(blob.data(), static_cast<long>(blob.size()));
    if (read != static_cast<long>(blob.size())) continue;

    const auto it = std::search(blob.begin(), blob.end(), std::begin(kMarker),
                                std::end(kMarker) - 1);
    if (it == blob.end()) continue;

    std::vector<std::string> names;
    std::size_t cursor =
        static_cast<std::size_t>(std::distance(blob.begin(), it) + (sizeof(kMarker) - 1));
    while (cursor < blob.size() && (blob[cursor] < 0x20 || blob[cursor] > 0x7E)) {
      ++cursor;
    }
    while (cursor < blob.size()) {
      std::size_t end = cursor;
      while (end < blob.size() && blob[end] != 0) {
        ++end;
      }
      if (end == cursor) {
        ++cursor;
        continue;
      }
      bool printable = true;
      for (std::size_t pos = cursor; pos < end; ++pos) {
        if (blob[pos] < 0x20 || blob[pos] > 0x7E) {
          printable = false;
          break;
        }
      }
      if (printable) {
        names.emplace_back(reinterpret_cast<const char*>(&blob[cursor]), end - cursor);
      }
      cursor = end + 1;
    }

    if (names.size() == static_cast<std::size_t>(Count)) {
      NameToCrc.clear();
      for (std::size_t idx = 0; idx < names.size(); ++idx) {
        NameToCrc[Upper_Name(names[idx].c_str())] = original_blocks[idx].CRC;
      }
      break;
    }
  }

  file.Close();
}
