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
  const size_t cols_size = static_cast<size_t>(cols);
  const size_t rows_size = static_cast<size_t>(rows);
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        const size_t idx = static_cast<size_t>(row) * cols_size + static_cast<size_t>(col);
        if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) {
          b[idx] = a[idx];
        } else {
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - cols_size] + a[idx + cols_size]) /
                   8u;
        }
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}