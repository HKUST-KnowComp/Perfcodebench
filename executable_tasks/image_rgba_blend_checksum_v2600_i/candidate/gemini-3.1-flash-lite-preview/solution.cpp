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

            // Fast approximation: (s * a + d * (255 - a) + 127) / 255
            // Using (x * 257 + 128) >> 16 is a standard fast division by 255
            for (int j = 0; j < 3; ++j) {
                uint32_t s = src[idx + j];
                uint32_t d = dst[idx + j];
                out[idx + j] = static_cast<uint8_t>((s * a + d * inv_a + 127) / 255);
            }
            out[idx + 3] = 255;
        }

        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_bytes; ++i) {
            hash ^= static_cast<uint64_t>(out[i]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}