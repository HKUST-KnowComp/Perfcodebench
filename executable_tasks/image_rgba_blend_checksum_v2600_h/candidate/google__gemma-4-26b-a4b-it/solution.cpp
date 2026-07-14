#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

// Fast integer division by 255: (x + 1 + (x >> 8)) >> 8 is a common trick,
// but for exactness with the baseline's (val + 127) / 255, we use:
// (val + 127 + (val + 127) >> 8) >> 8 is not quite right for all ranges.
// The baseline uses: (s*a + d*(255-a) + 127) / 255.
// Let X = s*a + d*(255-a) + 127.
// X / 255 ≈ (X + 1 + (X >> 8)) >> 8 is for X/256.
// For exactness with the baseline, we use the property that 
// (X + 127 + (X + 127) >> 8) >> 8 is not quite it. 
// Let's use the standard fast division: (X + 1 + (X >> 8)) >> 8 is for 256.
// Actually, the most robust way to match the baseline exactly while being fast
// is to use the identity: X / 255 = (X + 1 + (X >> 8)) >> 8 is for 256.
// For 255: X / 255 ≈ (X + 1 + (X >> 8)) >> 8 is close but not exact.
// However, the baseline is (val + 127) / 255. 
// Let's use: (val + 127 + (val + 127) >> 8) >> 8 is for 256.
// To be perfectly safe and fast, we use: 
// uint32_t res = (val + 127);
// return (uint8_t)((res + (res >> 8) + (res >> 16)) >> 8); // This is for 255
// Wait, the baseline is (s*a + d*(255-a) + 127) / 255.
// Let's use the bitwise trick for division by 255: (x + (x >> 8) + (x >> 16) + (x >> 24)) >> 8 is not quite it.
// The most accurate fast way: (x + 1 + (x >> 8)) >> 8 is for 256.
// Let's just use the division but ensure the compiler optimizes it, 
// or use: (x + 127 + (x + 127) >> 8) >> 8 is for 256.
// Actually, the baseline is simple enough that modern compilers optimize / 255 to a multiply-shift.
// The real bottleneck is the push_back and the checksum.

inline uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = bytes.data();
    const size_t size = bytes.size();
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
            const uint32_t s0 = src_ptr[idx];
            const uint32_t s1 = src_ptr[idx + 1];
            const uint32_t s2 = src_ptr[idx + 2];
            const uint32_t a  = src_ptr[idx + 3];
            
            const uint32_t d0 = dst_ptr[idx];
            const uint32_t d1 = dst_ptr[idx + 1];
            const uint32_t d2 = dst_ptr[idx + 2];

            const uint32_t inv_a = 255u - a;

            // Exact match for (s*a + d*(255-a) + 127) / 255
            out_ptr[idx]     = static_cast<uint8_t>((s0 * a + d0 * inv_a + 127u) / 255u);
            out_ptr[idx + 1] = static_cast<uint8_t>((s1 * a + d1 * inv_a + 127u) / 255u);
            out_ptr[idx + 2] = static_cast<uint8_t>((s2 * a + d2 * inv_a + 127u) / 255u);
            out_ptr[idx + 3] = 255u;
        }
        last_hash = checksum_bytes(out);
    }

    return last_hash;
}
