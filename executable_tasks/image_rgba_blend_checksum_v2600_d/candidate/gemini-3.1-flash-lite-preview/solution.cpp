#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t blend_checksum(const std::vector<uint8_t>& src, const std::vector<uint8_t>& dst, int width, int height, int iters) {
    const size_t num_pixels = static_cast<size_t>(width) * height;
    const size_t num_bytes = num_pixels * 4;
    std::vector<uint8_t> out(num_bytes);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < num_pixels; ++i) {
            const size_t idx = i * 4;
            const uint32_t a = src[idx + 3];
            const uint32_t inv_a = 255 - a;

            // Using (x * a + d * (255 - a) + 128) >> 8 is a standard fast approximation for / 255
            out[idx]     = static_cast<uint8_t>((src[idx] * a + dst[idx] * inv_a + 128) >> 8);
            out[idx + 1] = static_cast<uint8_t>((src[idx + 1] * a + dst[idx + 1] * inv_a + 128) >> 8);
            out[idx + 2] = static_cast<uint8_t>((src[idx + 2] * a + dst[idx + 2] * inv_a + 128) >> 8);
            out[idx + 3] = 255;
        }

        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < num_bytes; ++i) {
            h ^= static_cast<uint64_t>(out[i]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}