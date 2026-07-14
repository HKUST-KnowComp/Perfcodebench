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
    if (iters <= 0) return 0;

    const size_t pixels = src.size() / 4;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    const uint8_t* s = src.data();
    const uint8_t* d = dst.data();

    for (size_t i = 0; i < pixels; ++i) {
        size_t idx = i * 4;
        uint8_t a = s[idx + 3];
        uint8_t inv_a = 255 - a;

        uint32_t r = (uint32_t)s[idx] * a + (uint32_t)d[idx] * inv_a;
        hash ^= (uint8_t)((r + 127) / 255);
        hash *= FNV_PRIME;

        uint32_t g = (uint32_t)s[idx + 1] * a + (uint32_t)d[idx + 1] * inv_a;
        hash ^= (uint8_t)((g + 127) / 255);
        hash *= FNV_PRIME;

        uint32_t b = (uint32_t)s[idx + 2] * a + (uint32_t)d[idx + 2] * inv_a;
        hash ^= (uint8_t)((b + 127) / 255);
        hash *= FNV_PRIME;

        hash ^= 255;
        hash *= FNV_PRIME;
    }
    return hash;
}