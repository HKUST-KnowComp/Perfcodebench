#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// The original formula is: (s * a + d * (255 - a) + 127) / 255
// This can be rewritten to avoid division by 255 using the identity:
// (x / 255) ≈ ((x + 1 + (x >> 8)) >> 8)
// However, to guarantee EXACT correctness as per the requirement, 
// we must use the integer division or a perfectly equivalent bit-shift sequence.
// Given the requirement for exactness, we use the integer math directly.

inline void blend_pixel(const uint8_t* s, const uint8_t* d, uint8_t* o, uint8_t a) {
    // s[0]=R, s[1]=G, s[2]=B, s[3]=A
    // d[0]=R, d[1]=G, d[2]=B, d[3]=A
    // out[0]=R, out[1]=G, out[2]=B, out[3]=255
    
    uint32_t inv_a = 255u - a;
    
    o[0] = static_cast<uint8_t>((static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inv_a + 127u) / 255u);
    o[1] = static_cast<uint8_t>((static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inv_a + 127u) / 255u);
    o[2] = static_cast<uint8_t>((static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inv_a + 127u) / 255u);
    o[3] = 255u;
}

inline uint64_t checksum_bytes(const uint8_t* bytes, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(bytes[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    const size_t total_bytes = src.size();
    const size_t pixels = total_bytes / 4u;
    
    std::vector<uint8_t> out(total_bytes);
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s_ptr = src.data();
        const uint8_t* d_ptr = dst.data();
        uint8_t* o_ptr = out.data();

        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i * 4u;
            uint8_t a = s_ptr[idx + 3u];
            
            // Manual unrolling for compiler optimization
            uint32_t inv_a = 255u - a;
            o_ptr[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[idx])     * a + static_cast<uint32_t>(d_ptr[idx])     * inv_a + 127u) / 255u);
            o_ptr[idx + 1] = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[idx + 1]) * a + static_cast<uint32_t>(d_ptr[idx + 1]) * inv_a + 127u) / 255u);
            o_ptr[idx + 2] = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[idx + 2]) * a + static_cast<uint32_t>(d_ptr[idx + 2]) * inv_a + 127u) / 255u);
            o_ptr[idx + 3] = 255u;
        }
        last_hash = checksum_bytes(out.data(), total_bytes);
    }

    return last_hash;
}
