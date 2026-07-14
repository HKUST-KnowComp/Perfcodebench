#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <cmath>

namespace {

// Optimized Sobel kernel calculation
inline uint8_t compute_sobel(const uint8_t* p0, const uint8_t* p1, const uint8_t* p2) {
    // p0: row y-1, p1: row y, p2: row y+1
    // Indices relative to the start of the 3x3 window
    // p0[0] p0[1] p0[2]
    // p1[0] p1[1] p1[2]
    // p2[0] p2[1] p2[2]
    
    // gx = -p0[0] + p0[2] - 2*p1[0] + 2*p1[2] - p2[0] + p2[2]
    const int gx = -p0[0] + p0[2] - 2 * p1[0] + 2 * p1[2] - p2[0] + p2[2];
    // gy = p0[0] + 2*p0[1] + p0[2] - p2[0] - 2*p2[1] - p2[2]
    const int gy = p0[0] + 2 * p0[1] + p0[2] - p2[0] - 2 * p2[1] - p2[2];
    
    const int mag = std::abs(gx) + std::abs(gy);
    return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

} // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
    
    if (width < 3 || height < 3) return 0;

    const int out_width = width - 2;
    const int out_height = height - 2;
    const size_t out_size = static_cast<size_t>(out_width) * out_height;
    
    // Pre-allocate buffer to avoid repeated allocations in the loop
    std::vector<uint8_t> out_buffer(out_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* out_ptr = out_buffer.data();
        const uint8_t* img_ptr = image.data();

        for (int y = 1; y < height - 1; ++y) {
            const uint8_t* row_prev = &img_ptr[(y - 1) * width];
            const uint8_t* row_curr = &img_ptr[y * width];
            const uint8_t* row_next = &img_ptr[(y + 1) * width];

            for (int x = 1; x < width - 1; ++x) {
                // Pass pointers to the start of the 3x3 window for the current pixel
                *out_ptr++ = compute_sobel(&row_prev[x - 1], &row_curr[x - 1], &row_next[x - 1]);
            }
        }

        // Compute checksum over the resulting edge map
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < out_size; ++i) {
            hash ^= static_cast<uint64_t>(out_buffer[i]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}