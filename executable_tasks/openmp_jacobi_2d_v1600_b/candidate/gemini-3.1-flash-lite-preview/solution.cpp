#include "interface.h"
#include <omp.h>
#include <vector>
#include <cstdint>

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
    uint32_t* src = buffer1.data();
    uint32_t* dst = buffer2.data();

    for (int step = 0; step < steps; ++step) {
        #pragma omp parallel for schedule(static)
        for (int row = 1; row < rows - 1; ++row) {
            const uint32_t* prev_row = &src[(row - 1) * cols];
            const uint32_t* curr_row = &src[row * cols];
            const uint32_t* next_row = &src[(row + 1) * cols];
            uint32_t* out_row = &dst[row * cols];

            out_row[0] = curr_row[0];
            for (int col = 1; col < cols - 1; ++col) {
                out_row[col] = (curr_row[col] * 4u + curr_row[col - 1] + curr_row[col + 1] + prev_row[col] + next_row[col]) / 8u;
            }
            out_row[cols - 1] = curr_row[cols - 1];
        }
        
        // Copy boundaries
        for (int col = 0; col < cols; ++col) {
            dst[col] = src[col];
            dst[(rows - 1) * cols + col] = src[(rows - 1) * cols + col];
        }

        std::swap(src, dst);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < input.size(); ++i) {
        hash ^= static_cast<uint64_t>(src[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}