#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Optimized blend: (s*a + d*(255-a) + 127) / 255 = d + ((s-d)*a + 127) / 255
inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    return static_cast<uint8_t>(d + (static_cast<int32_t>(s - d) * a + 127) / 255);
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
    const size_t pixels = size / 4;
    
    // Pre-allocate output buffer to avoid repeated allocations in the loop
    std::vector<uint8_t> out(size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* __restrict out_ptr = out.data();
        const uint8_t* __restrict src_ptr = src.data();
        const uint8_t* __restrict dst_ptr = dst.data();

        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i * 4;
            const uint8_t a = src_ptr[idx + 3];
            
            out_ptr[idx]     = blend_byte(src_ptr[idx],     dst_ptr[idx],     a);
            out_ptr[idx + 1] = blend_byte(src_ptr[idx + 1], dst_ptr[idx + 1], a);
            out_ptr[idx + 2] = blend_byte(src_ptr[idx + 2], dst_ptr[idx + 2], a);
            out_ptr[idx + 3] = 255;
        }

        // Compute FNV-1a checksum
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < size; ++i) {
            current_hash ^= static_cast<uint64_t>(out_ptr[i]);
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    return hash;
}