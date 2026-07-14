#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast blending: (s*a + d*(255-a) + 127) / 255 using multiply-and-shift
inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t n = static_cast<uint32_t>(s) * a +
                  static_cast<uint32_t>(d) * (255u - a);
    uint32_t t = n + 127u;
    // 32897 = round(2^23 / 255), shift by 23 recovers floor(t/255)
    uint32_t result = (t * 32897u) >> 23;
    return static_cast<uint8_t>(result);
}

} // anonymous namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {

    int num_pixels = width * height;
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();

    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        const uint8_t* s = src_ptr;
        const uint8_t* d = dst_ptr;

        for (int i = 0; i < num_pixels; ++i) {
            uint8_t a = s[3];

            uint8_t b0 = blend_byte(s[0], d[0], a);
            uint8_t b1 = blend_byte(s[1], d[1], a);
            uint8_t b2 = blend_byte(s[2], d[2], a);
            uint8_t b3 = 255u;

            hash ^= b0; hash *= FNV_PRIME;
            hash ^= b1; hash *= FNV_PRIME;
            hash ^= b2; hash *= FNV_PRIME;
            hash ^= b3; hash *= FNV_PRIME;

            s += 4;
            d += 4;
        }

        final_hash = hash;
    }

    return final_hash;
}
