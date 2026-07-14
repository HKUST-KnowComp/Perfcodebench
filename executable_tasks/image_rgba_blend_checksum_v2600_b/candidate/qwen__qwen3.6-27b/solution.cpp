#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    if (iters <= 0) return 0;
    (void)width;
    (void)height;
    const size_t pixels = src.size() / 4;
    const uint8_t* s_ptr = src.data();
    const uint8_t* d_ptr = dst.data();

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < pixels; ++i) {
        uint8_t a = s_ptr[3];
        uint8_t inv_a = 255 - a;

        uint32_t r_num = static_cast<uint32_t>(s_ptr[0]) * a + static_cast<uint32_t>(d_ptr[0]) * inv_a + 127;
        uint32_t g_num = static_cast<uint32_t>(s_ptr[1]) * a + static_cast<uint32_t>(d_ptr[1]) * inv_a + 127;
        uint32_t b_num = static_cast<uint32_t>(s_ptr[2]) * a + static_cast<uint32_t>(d_ptr[2]) * inv_a + 127;

        uint8_t r = static_cast<uint8_t>(r_num / 255);
        uint8_t g = static_cast<uint8_t>(g_num / 255);
        uint8_t b = static_cast<uint8_t>(b_num / 255);

        hash ^= r; hash *= fnv_prime;
        hash ^= g; hash *= fnv_prime;
        hash ^= b; hash *= fnv_prime;
        hash ^= 255; hash *= fnv_prime;

        s_ptr += 4;
        d_ptr += 4;
    }
    return hash;
}