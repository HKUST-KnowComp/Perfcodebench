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
  std::vector<uint32_t> b(a.size());

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const std::size_t base = static_cast<std::size_t>(row) * cols;
      if (row == 0 || row == rows - 1) {
        // Boundary row: copy entire row from previous iteration
        for (int col = 0; col < cols; ++col) {
          b[base + col] = a[base + col];
        }
      } else {
        // Interior row: handle left boundary
        b[base] = a[base];
        // Interior columns (compute stencil)
        #pragma omp simd
        for (int col = 1; col < cols - 1; ++col) {
          const std::size_t idx = base + col;
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - static_cast<std::size_t>(cols)] +
                    a[idx + static_cast<std::size_t>(cols)]) /
                   8u;
        }
        // Right boundary
        b[base + cols - 1] = a[base + cols - 1];
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}