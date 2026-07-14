#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast approximation of (s * a + d * (255 - a) + 127) / 255
// Using the property: (x + 127) / 255 is approximately (x + 128) >> 8
// However, for exact match with the baseline's (s*a + d*(255-a) + 127)/255:
inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t res = (static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a) + 127u);
    // Division by 255 can be optimized, but for correctness we use the exact formula
    return static_cast<uint8_t>(res / 255u);
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
        uint64_t hash = FNV_OFFSET_BASIS;
        
        for (size_t i = 0; i < size; i += 4) {
            const uint8_t a = src_ptr[i + 3];
            
            // Blend RGB
            const uint8_t r = blend_fast(src_ptr[i], dst_ptr[i], a);
            const uint8_t g = blend_fast(src_ptr[i + 1], dst_ptr[i + 1], a);
            const uint8_t b = blend_fast(src_ptr[i + 2], dst_ptr[i + 2], a);
            const uint8_t alpha_out = 255u;

            // Update output buffer
            out_ptr[i] = r;
            out_ptr[i + 1] = g;
            out_ptr[i + 2] = b;
            out_ptr[i + 3] = alpha_out;

            // Inline checksum calculation to avoid second pass
            hash ^= static_cast<uint64_t>(r);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(g);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(b);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(alpha_out);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    
    return final_hash;
}