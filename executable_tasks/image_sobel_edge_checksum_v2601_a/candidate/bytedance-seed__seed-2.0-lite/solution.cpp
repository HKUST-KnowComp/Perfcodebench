#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

inline uint8_t blend(const uint8_t* row_above, const uint8_t* row_curr, const uint8_t* row_below, int x) {
    const int p0 = row_above[x-1];
    const int p1 = row_above[x];
    const int p2 = row_above[x+1];
    const int p3 = row_curr[x-1];
    const int p5 = row_curr[x+1];
    const int p6 = row_below[x-1];
    const int p7 = row_below[x];
    const int p8 = row_below[x+1];

    const int gx = -p0 + p2 - 2 * p3 + 2 * p5 - p6 + p8;
    const int gy = p0 + 2 * p1 + p2 - p6 - 2 * p7 - p8;
    const int mag = std::abs(gx) + std::abs(gy);
    return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

inline uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t b : bytes) {
        hash ^= static_cast<uint64_t>(b);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
    if (width < 3 || height < 3 || iters <= 0) {
        return 0;
    }

    const std::size_t out_width = static_cast<std::size_t>(width) - 2;
    const std::size_t out_height = static_cast<std::size_t>(height) - 2;
    const std::size_t out_size = out_width * out_height;
    std::vector<uint8_t> out(out_size);
    const uint8_t* image_ptr = image.data();

    // Compute Sobel edge map once, reuse across all iterations
    for (std::size_t y = 1; y < static_cast<std::size_t>(height) - 1; ++y) {
        const uint8_t* row_above = image_ptr + (y-1)*static_cast<std::size_t>(width);
        const uint8_t* row_curr = image_ptr + y*static_cast<std::size_t>(width);
        const uint8_t* row_below = image_ptr + (y+1)*static_cast<std::size_t>(width);
        const std::size_t out_row_offset = (y-1)*out_width;

        for (std::size_t x = 1; x < static_cast<std::size_t>(width) - 1; ++x) {
            const std::size_t out_idx = out_row_offset + (x-1);
            out[out_idx] = blend(row_above, row_curr, row_below, static_cast<int>(x));
        }
    }

    // Run checksum for all required iterations, no redundant edge map recomputation
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_bytes(out);
    }

    return hash;
}