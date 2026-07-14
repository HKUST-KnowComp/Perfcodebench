#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t blend_checksum(const std::vector<uint8_t>& src, const std::vector<uint8_t>& dst, int width, int height, int iters) {
    const size_t num_pixels = static_cast<size_t>(width) * height;
    const size_t num_bytes = num_pixels * 4;
    std::vector<uint8_t> out(num_bytes);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s_ptr = src.data();
        const uint8_t* d_ptr = dst.data();
        uint8_t* o_ptr = out.data();
        
        for (size_t i = 0; i < num_pixels; ++i) {
            uint32_t a = s_ptr[3];
            uint32_t inv_a = 255 - a;
            
            // Fast approximation: (s*a + d*(255-a) + 127) / 255
            // Using (val * 257 + 128) >> 8 is a standard approximation for / 255
            o_ptr[0] = static_cast<uint8_t>((s_ptr[0] * a + d_ptr[0] * inv_a + 127) / 255);
            o_ptr[1] = static_cast<uint8_t>((s_ptr[1] * a + d_ptr[1] * inv_a + 127) / 255);
            o_ptr[2] = static_cast<uint8_t>((s_ptr[2] * a + d_ptr[2] * inv_a + 127) / 255);
            o_ptr[3] = 255;
            
            s_ptr += 4;
            d_ptr += 4;
            o_ptr += 4;
        }

        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_bytes; ++i) {
            hash ^= static_cast<uint64_t>(out[i]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}