#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: inspect_cps <file>\n";
        return 2;
    }
    std::ifstream f(argv[1], std::ios::binary);
    if (!f) {
        std::cerr << "Failed to open " << argv[1] << "\n";
        return 1;
    }
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (data.size() < 10) {
        std::cerr << "File too small" << std::endl;
        return 1;
    }
    uint16_t file_size = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
    uint16_t compression = static_cast<uint16_t>(data[2]) | (static_cast<uint16_t>(data[3]) << 8);
    uint32_t uncompressed = static_cast<uint32_t>(data[4]) | (static_cast<uint32_t>(data[5]) << 8) |
                          (static_cast<uint32_t>(data[6]) << 16) | (static_cast<uint32_t>(data[7]) << 24);
    uint16_t palette_size = static_cast<uint16_t>(data[8]) | (static_cast<uint16_t>(data[9]) << 8);

    std::cout << "file_size=" << file_size << " compression=" << compression
              << " uncompressed=" << uncompressed << " palette_size=" << palette_size << "\n";

    size_t palette_offset = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t);
    if (data.size() > palette_offset) {
        unsigned char marker = data[palette_offset];
        std::cout << "first palette byte: 0x" << std::hex << static_cast<int>(marker) << std::dec << "\n";
    }

    return 0;
}
