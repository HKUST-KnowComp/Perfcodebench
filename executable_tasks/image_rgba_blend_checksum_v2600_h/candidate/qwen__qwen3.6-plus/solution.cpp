#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    (void)width;
    (void)height;
    (void)iters;

    const std::size_t pixels = src.size() / 4;
    const uint8_t* s = src.data();
    const uint8_t* d = dst.data();
    const uint8_t* s_end = s + pixels * 4;

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (; s < s_end; s += 4, d += 4) {
        uint8_t a = s[3];
        uint8_t inv_a = 255 - a;

        uint8_t r = (s[0] * a + d[0] * inv_a + 127) / 255;
        uint8_t g = (s[1] * a + d[1] * inv_a + 127) / 255;
        uint8_t b = (s[2] * a + d[2] * inv_a + 127) / 255;

        hash ^= r; hash *= FNV_PRIME;
        hash ^= g; hash *= FNV_PRIME;
        hash ^= b; hash *= FNV_PRIME;
        hash ^= 255; hash *= FNV_PRIME;
    }

    return hash;
}