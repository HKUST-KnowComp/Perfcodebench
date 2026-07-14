#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast blend: (s * a + d * (255 - a) + 127) / 255
// This is equivalent to: d + ((s - d) * a + 127) / 255
inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    return static_cast<uint8_t>(d + ((static_cast<int32_t>(s) - d) * a + 127) / 255);
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
    
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
        // Process pixels
        for (; i < size; i += 4) {
            const uint8_t a = src_ptr[i + 3];
            out_ptr[i]     = blend_fast(src_ptr[i],     dst_ptr[i],     a);
            out_ptr[i + 1] = blend_fast(src_ptr[i + 1], dst_ptr[i + 1], a);
            out_ptr[i + 2] = blend_fast(src_ptr[i + 2], dst_ptr[i + 2], a);
            out_ptr[i + 3] = 255;
        }

        // Compute checksum over the output buffer
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t j = 0; j < size; ++j) {
            hash ^= static_cast<uint64_t>(out_ptr[j]);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}