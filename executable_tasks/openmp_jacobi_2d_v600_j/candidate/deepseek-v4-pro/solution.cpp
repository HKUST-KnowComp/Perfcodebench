#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

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
  const std::size_t col_step = static_cast<std::size_t>(cols);
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      std::size_t row_start = static_cast<std::size_t>(row) * col_step;
      if (row == 0 || row == rows - 1) {
        // border rows: just copy entire row
        for (int col = 0; col < cols; ++col) {
          b[row_start + col] = a[row_start + col];
        }
      } else {
        // left border
        b[row_start] = a[row_start];
        // interior columns
        for (int col = 1; col < cols - 1; ++col) {
          std::size_t idx = row_start + col;
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - col_step] + a[idx + col_step]) /
                   8u;
        }
        // right border (if cols > 1)
        if (cols > 1) {
          b[row_start + cols - 1] = a[row_start + cols - 1];
        }
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}
