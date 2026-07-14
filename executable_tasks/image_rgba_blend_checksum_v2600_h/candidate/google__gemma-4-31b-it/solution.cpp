#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast approximation of (s * a + d * (255 - a) + 127) / 255
// The formula (val + 127) / 255 can be approximated as (val + 1 + (val >> 8)) >> 8
// for 0 <= val <= 255*255.
inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a);
    return static_cast<uint8_t>((val + 127u + ((val + 127u) >> 8)) >> 8);
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
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        
        for (size_t i = 0; i < size; i += 4) {
            const uint8_t a = src[i + 3];
            
            // Blend R, G, B
            uint8_t r = fast_blend(src[i], dst[i], a);
            uint8_t g = fast_blend(src[i + 1], dst[i + 1], a);
            uint8_t b = fast_blend(src[i + 2], dst[i + 2], a);
            uint8_t alpha_out = 255;

            // Inline checksum calculation to avoid storing the image
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