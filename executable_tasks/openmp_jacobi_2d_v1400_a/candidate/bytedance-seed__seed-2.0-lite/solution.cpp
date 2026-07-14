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
  const size_t ncols = static_cast<size_t>(cols);

  // Copy boundary values once, they never change across all iterations
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) {
        const size_t idx = static_cast<size_t>(row) * ncols + static_cast<size_t>(col);
        b[idx] = a[idx];
      }
    }
  }

  for (int step = 0; step < steps; ++step) {
    // Parallelize inner row processing with static scheduling for balanced work distribution
    #pragma omp parallel for schedule(static) default(none) shared(a, b, rows, ncols)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * ncols;
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + static_cast<size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - ncols] + a[idx + ncols]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}