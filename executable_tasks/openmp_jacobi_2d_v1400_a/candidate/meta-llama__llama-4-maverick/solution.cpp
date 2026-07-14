#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

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
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  const std::size_t cols_s = static_cast<std::size_t>(cols);
  #pragma omp parallel for
  for (int step = 0; step < steps; ++step) {
    std::vector<uint32_t>& src = (step % 2 == 0) ? a : b;
    std::vector<uint32_t>& dst = (step % 2 == 0) ? b : a;
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t idx_base = static_cast<std::size_t>(row) * cols_s + 1;
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = idx_base + static_cast<std::size_t>(col - 1);
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                    src[idx - cols_s] + src[idx + cols_s]) /
                   8u;
      }
    }
    #pragma omp barrier
    if (step == steps - 1) {
      #pragma omp single
      {
        for (int row = 0; row < rows; ++row) {
          const std::size_t idx_base = static_cast<std::size_t>(row) * cols_s;
          for (int col = 0; col < cols; ++col) {
            const std::size_t idx = idx_base + static_cast<std::size_t>(col);
            if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) {
              dst[idx] = src[idx];
            }
          }
        }
      }
    }
  }
  return checksum_u32((steps % 2 == 0) ? a : b);
}