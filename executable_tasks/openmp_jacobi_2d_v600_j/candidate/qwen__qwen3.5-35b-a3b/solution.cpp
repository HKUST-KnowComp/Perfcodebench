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
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (row 0 and row rows-1)
    for (int col = 0; col < cols; ++col) {
      b[col] = a[col];
      if (rows > 1) {
        b[(rows - 1) * cols + col] = a[(rows - 1) * cols + col];
      }
    }
    // Process inner rows
    #pragma omp parallel for
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t row_offset = static_cast<std::size_t>(row) * cols;
      // Copy boundary columns (col 0 and col cols-1)
      b[row_offset] = a[row_offset];
      if (cols > 1) {
        b[row_offset + cols - 1] = a[row_offset + cols - 1];
      }
      // Process inner columns
      for (int col = 1; col < cols - 1; ++col) {
        std::size_t idx = row_offset + col;
        b[idx] = ((a[idx] << 2) + a[idx - 1] + a[idx + 1] +
                  a[idx - cols] +
                  a[idx + cols]) >> 3;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}