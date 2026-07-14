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

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 0 || cols <= 0) return 0;

  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  // Pre-calculate boundary values to avoid conditional checks in the hot loop
  // However, for a small square, the overhead of complex boundary logic might exceed benefits.
  // We will use a parallelized loop with a conditional check that is branch-predictor friendly.

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * static_cast<std::size_t>(cols);
      
      if (row == 0 || row == rows - 1) {
        // Entire row is boundary
        for (int col = 0; col < cols; ++col) {
          b[row_offset + col] = a[row_offset + col];
        }
      } else {
        // First and last column are boundaries
        b[row_offset] = a[row_offset];
        for (int col = 1; col < cols - 1; ++col) {
          const std::size_t idx = row_offset + col;
          // Jacobi update formula
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                    a[idx - static_cast<std::size_t>(cols)] + 
                    a[idx + static_cast<std::size_t>(cols)]) / 8u;
        }
        b[row_offset + cols - 1] = a[row_offset + cols - 1];
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}