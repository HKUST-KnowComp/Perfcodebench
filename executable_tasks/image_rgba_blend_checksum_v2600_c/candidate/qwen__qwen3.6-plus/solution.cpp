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
    if (iters <= 0 || src.empty()) return 0;

    const uint8_t* s = src.data();
    const uint8_t* d = dst.data();
    const size_t n = src.size();
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < n; i += 4) {
        uint8_t a = s[i+3];
        uint8_t inv_a = 255 - a;
        
        uint32_t r_val = s[i] * a + d[i] * inv_a + 127;
        uint32_t g_val = s[i+1] * a + d[i+1] * inv_a + 127;
        uint32_t b_val = s[i+2] * a + d[i+2] * inv_a + 127;
        
        hash ^= (r_val / 255); hash *= FNV_PRIME;
        hash ^= (g_val / 255); hash *= FNV_PRIME;
        hash ^= (b_val / 255); hash *= FNV_PRIME;
        hash ^= 255; hash *= FNV_PRIME;
    }
    return hash;
}
