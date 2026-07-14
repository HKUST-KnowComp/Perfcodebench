#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Fast division by 255: (val + 127) / 255 is approximately (val + 1 + (val >> 8)) >> 8
// For the specific formula (s*a + d*(255-a) + 127) / 255:
// Let x = s*a + d*(255-a). The result is (x + 127) / 255.
// A very accurate fast version for (x + 127) / 255 where x is in [0, 255*255] is:
// return (uint8_t)((x + 128) * 0x01010101 >> 24);
// However, the standard fast approximation for (x + 127) / 255 is (x + 127 + ((x + 127) >> 8)) >> 8.
// Let's use the precise integer math for correctness as per the baseline.

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t x = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a);
    return static_cast<uint8_t>((x + 127u) / 255u);
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
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        const uint64_t prime = 1099511628211ULL;
        
        for (size_t i = 0; i < size; i += 4) {
            const uint8_t a = src_ptr[i + 3];
            
            // R
            uint8_t r = blend_byte_fast(src_ptr[i], dst_ptr[i], a);
            hash ^= static_cast<uint64_t>(r);
            hash *= prime;
            
            // G
            uint8_t g = blend_byte_fast(src_ptr[i + 1], dst_ptr[i + 1], a);
            hash ^= static_cast<uint64_t>(g);
            hash *= prime;
            
            // B
            uint8_t b = blend_byte_fast(src_ptr[i + 2], dst_ptr[i + 2], a);
            hash ^= static_cast<uint64_t>(b);
            hash *= prime;
            
            // A (always 255)
            hash ^= static_cast<uint64_t>(255);
            hash *= prime;
        }
        final_hash = hash;
    }
    
    return final_hash;
}