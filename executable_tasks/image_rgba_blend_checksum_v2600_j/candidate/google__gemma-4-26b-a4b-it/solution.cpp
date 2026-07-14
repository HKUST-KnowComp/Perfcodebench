#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

// Fast integer division by 255 using fixed-point approximation:
// (x + 1 + (x >> 8)) >> 8 is a common trick for (x / 255) for 16-bit values.
// However, for the specific formula (s*a + d*(255-a) + 127) / 255,
// we can use the property that (val + 127 + (val + 127) >> 8) >> 8 is not quite right.
// A more robust way for (val / 255) is (val + 1 + (val >> 8)) >> 8.
inline uint8_t fast_blend(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t val = s * a + d * (255u - a) + 127u;
    return static_cast<uint8_t>((val + 1 + (val >> 8)) >> 8);
}

inline uint64_t checksum_bytes(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
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
    
    // Pre-allocate output buffer to avoid reallocations in the loop
    std::vector<uint8_t> out(total_bytes);
    uint8_t* out_ptr = out.data();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();

    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i << 2;
            const uint32_t a = src_ptr[idx + 3];
            
            // Manual unrolling of the 4 channels
            // Channel R
            uint32_t r_val = static_cast<uint32_t>(src_ptr[idx]) * a + 
                             static_cast<uint32_t>(dst_ptr[idx]) * (255u - a) + 127u;
            out_ptr[idx] = static_cast<uint8_t>((r_val + 1 + (r_val >> 8)) >> 8);
            
            // Channel G
            uint32_t g_val = static_cast<uint32_t>(src_ptr[idx + 1]) * a + 
                             static_cast<uint32_t>(dst_ptr[idx + 1]) * (255u - a) + 127u;
            out_ptr[idx + 1] = static_cast<uint8_t>((g_val + 1 + (g_val >> 8)) >> 8);
            
            // Channel B
            uint32_t b_val = static_cast<uint32_t>(src_ptr[idx + 2]) * a + 
                             static_cast<uint32_t>(dst_ptr[idx + 2]) * (255u - a) + 127u;
            out_ptr[idx + 2] = static_cast<uint8_t>((b_val + 1 + (b_val >> 8)) >> 8);
            
            // Channel A (always 255)
            out_ptr[idx + 3] = 255;
        }
        last_hash = checksum_bytes(out_ptr, total_bytes);
    }

    return last_hash;
}