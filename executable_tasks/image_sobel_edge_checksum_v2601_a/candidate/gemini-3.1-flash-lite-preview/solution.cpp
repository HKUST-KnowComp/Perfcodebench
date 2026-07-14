#include "interface.h"
#include <cstdint>
#include <vector>
#include <cmath>
#include <algorithm>

uint64_t sobel_edge_checksum(const std::vector<uint8_t>& image, int width, int height, int iters) {
    if (width < 3 || height < 3) return 0;

    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* img = image.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = 1469598103934665603ULL;
        for (int y = 1; y < height - 1; ++y) {
            const uint8_t* row0 = &img[(y - 1) * width];
            const uint8_t* row1 = &img[y * width];
            const uint8_t* row2 = &img[(y + 1) * width];

            for (int x = 1; x < width - 1; ++x) {
                int gx = -row0[x - 1] + row0[x + 1] 
                         - 2 * row1[x - 1] + 2 * row1[x + 1] 
                         - row2[x - 1] + row2[x + 1];
                int gy = row0[x - 1] + 2 * row0[x] + row0[x + 1] 
                         - row2[x - 1] - 2 * row2[x] - row2[x + 1];
                
                int mag = std::abs(gx) + std::abs(gy);
                uint8_t val = static_cast<uint8_t>(mag > 255 ? 255 : mag);

                current_hash ^= static_cast<uint64_t>(val);
                current_hash *= 1099511628211ULL;
            }
        }
        hash = current_hash;
    }
    return hash;
}