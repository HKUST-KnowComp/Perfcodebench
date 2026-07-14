#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

// FNV-1a style hash used in the baseline
uint64_t compute_final_checksum(const uint32_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    if (rows <= 0 || cols <= 0) return 0;
    
    size_t n = static_cast<size_t>(rows) * static_cast<size_t>(cols);
    std::vector<uint32_t> grid_a = input;
    std::vector<uint32_t> grid_b(n);

    uint32_t* a = grid_a.data();
    uint32_t* b = grid_b.data();

    for (int step = 0; step < steps; ++step) {
        // Handle boundaries: top and bottom rows
        #pragma omp parallel
        {
            #pragma omp for nowait
            for (int j = 0; j < cols; ++j) {
                b[j] = a[j];
                b[(rows - 1) * cols + j] = a[(rows - 1) * cols + j];
            }

            // Handle boundaries: left and right columns (excluding corners already handled)
            #pragma omp for nowait
            for (int i = 1; i < rows - 1; ++i) {
                b[i * cols] = a[i * cols];
                b[i * cols + (cols - 1)] = a[i * cols + (cols - 1)];
            }

            // Interior points
            #pragma omp for
            for (int i = 1; i < rows - 1; ++i) {
                const uint32_t* __restrict__ a_row = &a[i * cols];
                const uint32_t* __restrict__ a_prev = &a[(i - 1) * cols];
                const uint32_t* __restrict__ a_next = &a[(i + 1) * cols];
                uint32_t* __restrict__ b_row = &b[i * cols];

                for (int j = 1; j < cols - 1; ++j) {
                    b_row[j] = (a_row[j] * 4u + 
                                a_row[j - 1] + 
                                a_row[j + 1] + 
                                a_prev[j] + 
                                a_next[j]) / 8u;
                }
            }
        }
        
        // Swap pointers for next iteration
        uint32_t* temp = a;
        a = b;
        b = temp;
    }

    // The final result is in 'a' because of the swap at the end of the loop
    return compute_final_checksum(a, n);
}