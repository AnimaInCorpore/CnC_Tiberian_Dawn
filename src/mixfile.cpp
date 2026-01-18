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
    std::vector<MixEntry> entries_in_order;
    std::vector<MixEntry> entries_by_crc;
    std::vector<std::string> names_in_order;  // Uppercase; optional (XCC database).
};

struct MixEntryLessByCRC {
    bool operator()(MixEntry const& a, MixEntry const& b) const { return a.crc < b.crc; }
};

static bool g_scanned = false;
static std::vector<MixArchive> g_archives;
static std::map<std::string, void const*> g_cache;

static std::string Normalize_Name(const char* filename) {
    if (!filename) return std::string();
    std::string out(filename);
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
    }
    return out;
}

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
    std::string upper = Normalize_Name(filename);
    return Calculate_CRC(upper.c_str(), static_cast<int>(upper.size()));
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

static std::vector<std::string> Parse_XCC_Names(unsigned char const* data, std::size_t size, std::size_t expected_count) {
    std::vector<std::string> names;
    if (!data || size < 8) return names;
    if (size < 4 || std::memcmp(data, "XCC ", 4) != 0) return names;

    // XCC mix metadata contains a binary header followed by a series of
    // NUL-terminated filenames (often lowercase). Extract all plausible
    // ASCII filename strings and accept them when the count matches.
    for (std::size_t i = 0; i < size; ++i) {
        unsigned char c = data[i];
        if (c < 32 || c >= 127) continue;

        std::size_t j = i;
        while (j < size) {
            unsigned char cj = data[j];
            if (cj == 0) break;
            if (cj < 32 || cj >= 127) {
                j = size;
                break;
            }
            ++j;
        }
        if (j >= size || data[j] != 0) continue;

        std::size_t len = j - i;
        if (len < 5 || len >= 64) continue;
        bool has_dot = false;
        for (std::size_t k = i; k < j; ++k) {
            if (data[k] == '.') has_dot = true;
        }
        if (!has_dot) continue;

        std::string s(reinterpret_cast<char const*>(data + i), len);
        names.push_back(Normalize_Name(s.c_str()));
        i = j;
    }

    if (expected_count && names.size() != expected_count) {
        names.clear();
    }
    return names;
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
        archive.entries_in_order.reserve(count);

        for (unsigned short i = 0; i < count; ++i) {
            unsigned long crc = 0;
            unsigned long off = 0;
            unsigned long size = 0;
            if (!Read_LE32(fp, crc) || !Read_LE32(fp, off) || !Read_LE32(fp, size)) {
                archive.entries_in_order.clear();
                break;
            }
            MixEntry entry;
            entry.crc = crc;
            entry.offset = static_cast<long>(off);
            entry.size = static_cast<long>(size);
            archive.entries_in_order.push_back(entry);
        }

        long header_size = 2 + 4 + static_cast<long>(count) * 12;
        if (!archive.entries_in_order.empty()) {
            // Optional: XCC-generated MIX files include an extra entry with
            // the original filenames, allowing name-based lookups without
            // relying on the legacy CRC algorithm.
            for (std::size_t i = 0; i < archive.entries_in_order.size(); ++i) {
                long abs_off = header_size + archive.entries_in_order[i].offset;
                long size = archive.entries_in_order[i].size;
                if (size <= 0 || size > 2048) continue;
                if (std::fseek(fp, abs_off, SEEK_SET) != 0) continue;
                unsigned char sig[4];
                if (std::fread(sig, 1, 4, fp) != 4) continue;
                if (std::memcmp(sig, "XCC ", 4) != 0) continue;

                std::vector<unsigned char> buf(static_cast<std::size_t>(size));
                std::fseek(fp, abs_off, SEEK_SET);
                if (std::fread(&buf[0], 1, static_cast<std::size_t>(size), fp) != static_cast<std::size_t>(size)) break;

                archive.names_in_order = Parse_XCC_Names(&buf[0], buf.size(), archive.entries_in_order.size());
                break;
            }
        }

        std::fclose(fp);

        if (archive.entries_in_order.empty()) continue;

        archive.entries_by_crc = archive.entries_in_order;
        std::sort(archive.entries_by_crc.begin(), archive.entries_by_crc.end(), MixEntryLessByCRC());

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
    if (archive.entries_by_crc.empty()) return false;

    std::size_t lo = 0;
    std::size_t hi = archive.entries_by_crc.size();
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        unsigned long mid_crc = archive.entries_by_crc[mid].crc;
        if (mid_crc < crc) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo >= archive.entries_by_crc.size() || archive.entries_by_crc[lo].crc != crc) return false;

    long header_size = 2 + 4 + static_cast<long>(archive.entries_by_crc.size()) * 12;
    out_absolute_offset = header_size + archive.entries_by_crc[lo].offset;
    out_size = archive.entries_by_crc[lo].size;
    return true;
}

}  // namespace

namespace MixFileClass {

void const* Retrieve(const char* filename) {
    if (!filename) return 0;
    Ensure_Scanned();

    std::string normalized = Normalize_Name(filename);
    std::map<std::string, void const*>::const_iterator cached = g_cache.find(normalized);
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

    g_cache[normalized] = buffer;
    return buffer;
}

bool Offset(char const* filename, void** realptr, char const** mix_filename, long* offset, long* size) {
    if (!filename) return false;
    Ensure_Scanned();

    if (realptr) *realptr = 0;
    if (mix_filename) *mix_filename = 0;
    if (offset) *offset = 0;
    if (size) *size = 0;

    std::string normalized = Normalize_Name(filename);

    std::map<std::string, void const*>::const_iterator cached = g_cache.find(normalized);
    if (cached != g_cache.end()) {
        if (realptr) *realptr = const_cast<void*>(cached->second);
        return true;
    }

    // Prefer filename tables embedded by XCC tools when present.
    for (std::size_t i = 0; i < g_archives.size(); ++i) {
        MixArchive const& archive = g_archives[i];
        if (archive.names_in_order.size() != archive.entries_in_order.size()) continue;
        for (std::size_t j = 0; j < archive.names_in_order.size(); ++j) {
            if (archive.names_in_order[j] != normalized) continue;
            long header_size = 2 + 4 + static_cast<long>(archive.entries_in_order.size()) * 12;
            long abs_off = header_size + archive.entries_in_order[j].offset;
            long file_size = archive.entries_in_order[j].size;
            if (mix_filename) *mix_filename = archive.path.c_str();
            if (offset) *offset = abs_off;
            if (size) *size = file_size;
            return true;
        }
    }

    unsigned long crc = Filename_CRC(normalized.c_str());
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
