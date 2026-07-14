#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    if (rows <= 2 || cols <= 2) {
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t v : input) {
            hash ^= static_cast<uint64_t>(v);
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    std::vector<uint32_t> buffer1 = input;
    std::vector<uint32_t> buffer2(input.size());
    uint32_t* a = buffer1.data();
    uint32_t* b = buffer2.data();

    for (int step = 0; step < steps; ++step) {
        #pragma omp parallel for schedule(static)
        for (int row = 1; row < rows - 1; ++row) {
            const uint32_t* row_prev = &a[(row - 1) * cols];
            const uint32_t* row_curr = &a[row * cols];
            const uint32_t* row_next = &a[(row + 1) * cols];
            uint32_t* row_out = &b[row * cols];

            row_out[0] = row_curr[0];
            for (int col = 1; col < cols - 1; ++col) {
                row_out[col] = (row_curr[col] * 4u + row_curr[col - 1] + row_curr[col + 1] + row_prev[col] + row_next[col]) / 8u;
            }
            row_out[cols - 1] = row_curr[cols - 1];
        }

        // Copy boundaries
        for (int col = 0; col < cols; ++col) {
            b[col] = a[col];
            b[(rows - 1) * cols + col] = a[(rows - 1) * cols + col];
        }

        std::swap(a, b);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < input.size(); ++i) {
        hash ^= static_cast<uint64_t>(a[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}