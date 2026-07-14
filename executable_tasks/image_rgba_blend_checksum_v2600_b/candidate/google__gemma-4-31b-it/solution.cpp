#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast implementation of (s * a + d * (255 - a) + 127) / 255
// This is equivalent to: (s * a + d * 255 - d * a + 127) / 255
// = d + (a * (s - d) + 127) / 255
inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    int32_t diff = static_cast<int32_t>(s) - static_cast<int32_t>(d);
    int32_t term = (static_cast<int32_t>(a) * diff) + 127;
    // Exact division by 255: (term + 1 + (term >> 8)) >> 8 is for 0-255 range
    // For signed values and the specific +127 offset, we use standard division
    // as compilers optimize /255 into a multiply-shift sequence.
    return static_cast<uint8_t>(static_cast<int32_t>(d) + (term / 255));
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    (void)width;
    (void)height;
    
    const size_t size = src.size();
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    
    // Pre-allocate output buffer to avoid repeated allocations
    std::vector<uint8_t> out(size);
    uint8_t* out_ptr = out.data();
    
    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        for (; i + 3 < size; i += 4) {
            const uint8_t a = src_ptr[i + 3];
            out_ptr[i]     = blend_fast(src_ptr[i],     dst_ptr[i],     a);
            out_ptr[i + 1] = blend_fast(src_ptr[i + 1], dst_ptr[i + 1], a);
            out_ptr[i + 2] = blend_fast(src_ptr[i + 2], dst_ptr[i + 2], a);
            out_ptr[i + 3] = 255;
        }

        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t j = 0; j < size; ++j) {
            hash ^= static_cast<uint64_t>(out_ptr[j]);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}