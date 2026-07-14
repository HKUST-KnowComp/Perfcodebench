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
    const std::size_t n = src.size();
    if (n == 0 || iters == 0) return 0;

    const uint8_t* s = src.data();
    const uint8_t* d = dst.data();
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; i += 4) {
            uint8_t a = s[i + 3];
            uint8_t ia = 255u - a;

            uint32_t r_val = static_cast<uint32_t>(s[i]) * a + static_cast<uint32_t>(d[i]) * ia + 127u;
            uint32_t g_val = static_cast<uint32_t>(s[i+1]) * a + static_cast<uint32_t>(d[i+1]) * ia + 127u;
            uint32_t b_val = static_cast<uint32_t>(s[i+2]) * a + static_cast<uint32_t>(d[i+2]) * ia + 127u;

            hash ^= static_cast<uint8_t>(r_val / 255u); hash *= prime;
            hash ^= static_cast<uint8_t>(g_val / 255u); hash *= prime;
            hash ^= static_cast<uint8_t>(b_val / 255u); hash *= prime;
            hash ^= 255u; hash *= prime;
        }
    }
    return hash;
}