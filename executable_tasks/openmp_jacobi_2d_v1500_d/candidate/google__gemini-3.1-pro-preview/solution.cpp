#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

/**
 * FNV-1a hash implementation for checksum calculation.
 * This is inherently sequential as each step depends on the previous hash value.
 */
uint64_t checksum_u32(const std::vector<uint32_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : data) {
        hash ^= static_cast<uint64_t>(v);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    if (rows <= 0 || cols <= 0) return 0;
    if (steps <= 0) return checksum_u32(input);

    // Use two buffers to avoid in-place update issues.
    std::vector<uint32_t> a = input;
    std::vector<uint32_t> b = input; // Initialize b with input to preserve boundaries

    const size_t r = static_cast<size_t>(rows);
    const size_t c = static_cast<size_t>(cols);

    // If the grid is too small to have an interior, no updates occur.
    if (r > 2 && c > 2) {
        for (int step = 0; step < steps; ++step) {
            uint32_t* __restrict__ src = a.data();
            uint32_t* __restrict__ dst = b.data();

            // Parallelize the interior row updates.
            // The boundary rows (0 and r-1) and boundary columns (0 and c-1) 
            // are never modified, so they remain as initialized from 'input'.
            #pragma omp parallel for schedule(static)
            for (int i = 1; i < (int)(r - 1); ++i) {
                const size_t offset = (size_t)i * c;
                const uint32_t* __restrict__ prev_row = &src[offset - c];
                const uint32_t* __restrict__ curr_row = &src[offset];
                const uint32_t* __restrict__ next_row = &src[offset + c];
                uint32_t* __restrict__ out_row = &dst[offset];

                // Vectorizable inner loop for the 5-point stencil.
                // Division by 8 is implemented as a bit shift for performance.
                #pragma omp simd
                for (int j = 1; j < (int)(c - 1); ++j) {
                    out_row[j] = (curr_row[j] * 4u + 
                                  curr_row[j - 1] + curr_row[j + 1] + 
                                  prev_row[j] + next_row[j]) >> 3;
                }
            }
            // Swap pointers for the next iteration.
            a.swap(b);
        }
    }

    return checksum_u32(a);
}