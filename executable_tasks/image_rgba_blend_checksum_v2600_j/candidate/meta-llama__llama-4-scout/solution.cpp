#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

const uint32_t kAlphaLookupTable[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    const uint32_t s_scaled = s * kAlphaLookupTable[a];
    const uint32_t d_scaled = d * (255 - kAlphaLookupTable[a]);
    return static_cast<uint8_t>((s_scaled + d_scaled + 127) >> 8);
}

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t b : bytes) {
        hash ^= static_cast<uint64_t>(b);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    (void)width;
    (void)height;
    const std::size_t pixels = src.size() / 4u;
    std::vector<uint8_t> out;
    out.reserve(pixels * 4);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        out.resize(pixels * 4);
        for (std::size_t i = 0; i < pixels; ++i) {
            const std::size_t idx = i * 4u;
            const uint8_t a = src[idx + 3u];
            out[idx] = blend_byte(src[idx], dst[idx], a);
            out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
            out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out);
    }
    return hash;
}