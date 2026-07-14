#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <cmath>

namespace {

inline uint8_t blend_optimized(const uint8_t* p) {
    // p is a pointer to the top-left pixel of a 3x3 neighborhood
    // Using direct indexing to avoid array copies
    // gx = -p[0] + p[2] - 2*p[3] + 2*p[5] - p[6] + p[8]
    // gy = p[0] + 2*p[1] + p[2] - p[6] - 2*p[7] - p[8]
    
    const int gx = -static_cast<int>(p[0]) + static_cast<int>(p[2]) 
                   - 2 * static_cast<int>(p[width_offset + 1]) // wait, p is local 3x3
                   // Let's use the actual logic from the baseline to ensure exactness
                   ; 
    // Re-implementing exactly as baseline but with direct pointer access
    return 0; // placeholder for logic below
}

// To ensure exact correctness, we will use the exact math from the baseline
// but optimize the loop and memory access.

inline uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t b : bytes) {
        hash ^= static_cast<uint64_t>(b);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
    
    if (width < 3 || height < 3) return 0; // Should not happen based on task

    const size_t img_size = static_cast<size_t>(width) * height;
    const uint8_t* img_ptr = image.data();
    
    // Pre-calculate output size: (width-2) * (height-2)
    const size_t out_size = static_cast<size_t>(width - 2) * (height - 2);
    std::vector<uint8_t> out(out_size);
    uint8_t* out_ptr = out.data();

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (int y = 1; y < height - 1; ++y) {
            const size_t row_prev = static_cast<size_t>(y - 1) * width;
            const size_t row_curr = static_cast<size_t>(y) * width;
            const size_t row_next = static_cast<size_t>(y + 1) * width;
            
            size_t out_row_idx = static_cast<size_t>(y - 1) * (width - 2);

            for (int x = 1; x < width - 1; ++x) {
                const size_t cur_x = static_cast<size_t>(x);
                
                // Extract 3x3 neighborhood values directly
                const int p0 = img_ptr[row_prev + cur_x - 1];
                const int p1 = img_ptr[row_prev + cur_x];
                const int p2 = img_ptr[row_prev + cur_x + 1];
                const int p3 = img_ptr[row_curr + cur_x - 1];
                const int p5 = img_ptr[row_curr + cur_x + 1];
                const int p6 = img_ptr[row_next + cur_x - 1];
                const int p7 = img_ptr[row_next + cur_x];
                const int p8 = img_ptr[row_next + cur_x + 1];

                const int gx = -p0 + p2 - 2 * p3 + 2 * p5 - p6 + p8;
                const int gy = p0 + 2 * p1 + p2 - p6 - 2 * p7 - p8;
                
                // std::abs is fine, but we can use a branchless or simple version
                int abs_gx = (gx < 0) ? -gx : gx;
                int abs_gy = (gy < 0) ? -gy : gy;
                int mag = abs_gx + abs_gy;
                
                out_ptr[out_row_idx + (x - 1)] = static_cast<uint8_t>(mag > 255 ? 255 : mag);
            }
        }
        
        // Checksum calculation
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < out_size; ++i) {
            hash ^= static_cast<uint64_t>(out_ptr[i]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}