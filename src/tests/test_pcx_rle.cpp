#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

static std::vector<unsigned char> Decode_Rle_Stream(const std::vector<unsigned char>& payload, int width, int height) {
    const std::size_t total_pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    std::vector<unsigned char> out(total_pixels, 0);
    std::size_t pos = 0;
    std::size_t out_pos = 0;
    const std::size_t pixel_data_size = payload.size();
    while (out_pos < total_pixels && pos < pixel_data_size) {
        unsigned char value = payload[pos++];
        std::size_t count = 1;
        if ((value & 0xC0) == 0xC0) {
            if (pos >= pixel_data_size) break;
            count = static_cast<std::size_t>(value & 0x3F);
            value = payload[pos++];
        }
        const std::size_t write_count = std::min(count, total_pixels - out_pos);
        std::fill_n(out.data() + out_pos, write_count, value);
        out_pos += write_count;
    }
    return out;
}

int main() {
    // Craft a payload with a single run of 7 pixels with value 9.
    // Width=5, Height=2 -> 10 pixels total. The run crosses the first scanline boundary.
    std::vector<unsigned char> payload = { static_cast<unsigned char>(0xC0 | 7), 9 };
    auto pixels = Decode_Rle_Stream(payload, 5, 2);

    // Expect first 7 entries to be 9, last 3 to remain 0.
    for (size_t i = 0; i < 7; ++i) {
        assert(pixels[i] == 9);
    }
    for (size_t i = 7; i < 10; ++i) {
        assert(pixels[i] == 0);
    }

    std::cout << "test_pcx_rle: OK\n";
    return 0;
}
