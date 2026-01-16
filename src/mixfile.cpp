#include "legacy_compat.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>

namespace {

struct MixEntry {
    unsigned long crc;
    long offset;
    long size;
};

struct MixArchive {
    std::string path;
    std::vector<MixEntry> entries;
};

struct MixEntryLessByCRC {
    bool operator()(MixEntry const& a, MixEntry const& b) const { return a.crc < b.crc; }
};

static bool g_scanned = false;
static std::vector<MixArchive> g_archives;
static std::map<std::string, void const*> g_cache;

static unsigned long Calculate_CRC(void const* buffer, int length) {
    unsigned long sum = 0;
    unsigned long hibit = 0;
    unsigned char const* ptr = static_cast<unsigned char const*>(buffer);
    for (int i = 0; i < length; ++i) {
        hibit = (sum & 0x80000000UL) ? 1UL : 0UL;
        sum <<= 1;
        sum += hibit + static_cast<unsigned long>(ptr[i]);
    }
    return sum;
}

static unsigned long Filename_CRC(char const* filename) {
    if (!filename) return 0;
    char tmp[260];
    std::size_t len = std::strlen(filename);
    if (len >= sizeof(tmp)) len = sizeof(tmp) - 1;
    for (std::size_t i = 0; i < len; ++i) {
        tmp[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(filename[i])));
    }
    tmp[len] = '\0';
    return Calculate_CRC(tmp, static_cast<int>(len));
}

static bool Read_LE16(std::FILE* fp, unsigned short& out) {
    unsigned char b[2];
    if (std::fread(b, 1, 2, fp) != 2) return false;
    out = static_cast<unsigned short>(b[0] | (static_cast<unsigned short>(b[1]) << 8));
    return true;
}

static bool Read_LE32(std::FILE* fp, unsigned long& out) {
    unsigned char b[4];
    if (std::fread(b, 1, 4, fp) != 4) return false;
    out = static_cast<unsigned long>(b[0]) |
          (static_cast<unsigned long>(b[1]) << 8) |
          (static_cast<unsigned long>(b[2]) << 16) |
          (static_cast<unsigned long>(b[3]) << 24);
    return true;
}

static bool Has_Extension(char const* name, char const* ext_upper) {
    std::size_t nlen = std::strlen(name);
    std::size_t elen = std::strlen(ext_upper);
    if (nlen < elen) return false;
    for (std::size_t i = 0; i < elen; ++i) {
        char c = name[nlen - elen + i];
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (c != ext_upper[i]) return false;
    }
    return true;
}

static bool Is_Directory(std::string const& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return (st.st_mode & S_IFDIR) != 0;
}

static void Scan_Directory(std::string const& dir, int depth_remaining) {
    DIR* dp = opendir(dir.c_str());
    if (!dp) return;

    for (;;) {
        struct dirent* ent = readdir(dp);
        if (!ent) break;
        if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0) continue;

        std::string path = dir;
        if (!path.empty() && path[path.size() - 1] != '/') path += '/';
        path += ent->d_name;

        if (Is_Directory(path)) {
            if (depth_remaining > 0) Scan_Directory(path, depth_remaining - 1);
            continue;
        }

        if (!Has_Extension(ent->d_name, ".MIX")) continue;

        std::FILE* fp = std::fopen(path.c_str(), "rb");
        if (!fp) continue;

        unsigned short count = 0;
        unsigned long data_size = 0;
        if (!Read_LE16(fp, count) || !Read_LE32(fp, data_size)) {
            std::fclose(fp);
            continue;
        }

        MixArchive archive;
        archive.path = path;
        archive.entries.reserve(count);

        for (unsigned short i = 0; i < count; ++i) {
            unsigned long crc = 0;
            unsigned long off = 0;
            unsigned long size = 0;
            if (!Read_LE32(fp, crc) || !Read_LE32(fp, off) || !Read_LE32(fp, size)) {
                archive.entries.clear();
                break;
            }
            MixEntry entry;
            entry.crc = crc;
            entry.offset = static_cast<long>(off);
            entry.size = static_cast<long>(size);
            archive.entries.push_back(entry);
        }

        std::fclose(fp);

        if (archive.entries.empty()) continue;

        std::sort(archive.entries.begin(), archive.entries.end(), MixEntryLessByCRC());

        g_archives.push_back(archive);
    }

    closedir(dp);
}

static void Ensure_Scanned() {
    if (g_scanned) return;
    g_scanned = true;

    Scan_Directory("CD", 3);
}

static bool Find_In_Archive(MixArchive const& archive,
                            unsigned long crc,
                            long& out_absolute_offset,
                            long& out_size) {
    if (archive.entries.empty()) return false;

    std::size_t lo = 0;
    std::size_t hi = archive.entries.size();
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        unsigned long mid_crc = archive.entries[mid].crc;
        if (mid_crc < crc) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo >= archive.entries.size() || archive.entries[lo].crc != crc) return false;

    long header_size = 2 + 4 + static_cast<long>(archive.entries.size()) * 12;
    out_absolute_offset = header_size + archive.entries[lo].offset;
    out_size = archive.entries[lo].size;
    return true;
}

}  // namespace

namespace MixFileClass {

void const* Retrieve(const char* filename) {
    if (!filename) return 0;
    Ensure_Scanned();

    std::map<std::string, void const*>::const_iterator cached = g_cache.find(filename);
    if (cached != g_cache.end()) return cached->second;

    void* realptr = 0;
    char const* mix_filename = 0;
    long offset = 0;
    long size = 0;

    if (!Offset(filename, &realptr, &mix_filename, &offset, &size)) return 0;
    if (realptr) return realptr;
    if (!mix_filename || size <= 0) return 0;

    std::FILE* fp = std::fopen(mix_filename, "rb");
    if (!fp) return 0;
    if (std::fseek(fp, offset, SEEK_SET) != 0) {
        std::fclose(fp);
        return 0;
    }

    unsigned char* buffer = static_cast<unsigned char*>(std::malloc(static_cast<std::size_t>(size)));
    if (!buffer) {
        std::fclose(fp);
        return 0;
    }
    if (std::fread(buffer, 1, static_cast<std::size_t>(size), fp) != static_cast<std::size_t>(size)) {
        std::free(buffer);
        std::fclose(fp);
        return 0;
    }
    std::fclose(fp);

    g_cache[filename] = buffer;
    return buffer;
}

bool Offset(char const* filename, void** realptr, char const** mix_filename, long* offset, long* size) {
    if (!filename) return false;
    Ensure_Scanned();

    if (realptr) *realptr = 0;
    if (mix_filename) *mix_filename = 0;
    if (offset) *offset = 0;
    if (size) *size = 0;

    std::map<std::string, void const*>::const_iterator cached = g_cache.find(filename);
    if (cached != g_cache.end()) {
        if (realptr) *realptr = const_cast<void*>(cached->second);
        return true;
    }

    unsigned long crc = Filename_CRC(filename);
    for (std::size_t i = 0; i < g_archives.size(); ++i) {
        long abs_off = 0;
        long file_size = 0;
        if (!Find_In_Archive(g_archives[i], crc, abs_off, file_size)) continue;

        if (mix_filename) *mix_filename = g_archives[i].path.c_str();
        if (offset) *offset = abs_off;
        if (size) *size = file_size;
        return true;
    }

    return false;
}

}  // namespace MixFileClass
