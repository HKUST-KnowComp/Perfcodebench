#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    std::vector<uint32_t> buffer1 = input;
    std::vector<uint32_t> buffer2(rows * cols);
    uint32_t* src = buffer1.data();
    uint32_t* dst = buffer2.data();

    for (int step = 0; step < steps; ++step) {
        #pragma omp parallel for schedule(static)
        for (int row = 0; row < rows; ++row) {
            if (row == 0 || row == rows - 1) {
                for (int col = 0; col < cols; ++col) {
                    dst[row * cols + col] = src[row * cols + col];
                }
            } else {
                dst[row * cols] = src[row * cols];
                for (int col = 1; col < cols - 1; ++col) {
                    size_t idx = (size_t)row * cols + col;
                    dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + 
                                src[idx - cols] + src[idx + cols]) / 8u;
                }
                dst[row * cols + cols - 1] = src[row * cols + cols - 1];
            }
        }
        std::swap(src, dst);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < rows * cols; ++i) {
        hash ^= static_cast<uint64_t>(src[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}