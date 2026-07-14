#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

// The original formula is: (s * a + d * (255 - a) + 127) / 255
// This can be rewritten to avoid division by 255 using the identity:
// (x / 255) ≈ (x + 128 + (x >> 8)) >> 8 is not quite right for exactness.
// To maintain EXACT correctness as required by the task, we must use the exact integer division.
// However, we can optimize the calculation of (s*a + d*(255-a) + 127) / 255.
// Let val = s*a + d*(255-a) + 127.
// We can use the property that x/255 = (x + 1 + (x >> 8)) >> 8 is an approximation.
// For exactness, we use: (val + (val >> 8) + 1) >> 8 is not quite it.
// The most efficient way to do exact division by 255 for a 32-bit integer:
// (val + 1 + (val >> 8)) >> 8 is actually for 256. 
// For 255: (val + 127 + (val >> 8)) >> 8 is also an approximation.
// Since the requirement is EXACT correctness, we must use the division or a very precise fixed-point equivalent.
// Let's use the division but optimize the loop structure and memory access.

inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t sa = static_cast<uint32_t>(s) * a;
    uint32_t da = static_cast<uint32_t>(d) * (255u - a);
    return static_cast<uint8_t>((sa + da + 127u) / 255u);
}

inline uint64_t checksum_bytes_optimized(const uint8_t* data, size_t size) {
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
    
    std::vector<uint8_t> out(total_bytes);
    uint8_t* out_ptr = out.data();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();

    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i * 4u;
            const uint8_t s0 = src_ptr[idx];
            const uint8_t s1 = src_ptr[idx + 1];
            const uint8_t s2 = src_ptr[idx + 2];
            const uint8_t a  = src_ptr[idx + 3];
            
            const uint8_t d0 = dst_ptr[idx];
            const uint8_t d1 = dst_ptr[idx + 1];
            const uint8_t d2 = dst_ptr[idx + 2];

            const uint32_t inv_a = 255u - a;

            out_ptr[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(s0) * a + static_cast<uint32_t>(d0) * inv_a + 127u) / 255u);
            out_ptr[idx + 1] = static_cast<uint8_t>((static_cast<uint32_t>(s1) * a + static_cast<uint32_t>(d1) * inv_a + 127u) / 255u);
            out_ptr[idx + 2] = static_cast<uint8_t>((static_cast<uint32_t>(s2) * a + static_cast<uint32_t>(d2) * inv_a + 127u) / 255u);
            out_ptr[idx + 3] = 255u;
        }
        last_hash = checksum_bytes_optimized(out_ptr, total_bytes);
    }

    return last_hash;
}
