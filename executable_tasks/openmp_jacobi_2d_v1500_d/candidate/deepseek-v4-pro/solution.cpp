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
  const size_t stride = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      size_t base = static_cast<size_t>(row) * stride;
      if (row == 0 || row == rows - 1) {
        // entire row is boundary: copy
        for (int col = 0; col < cols; ++col) {
          b[base + col] = a[base + col];
        }
      } else {
        // first column
        b[base] = a[base];
        // interior cells
        #pragma omp simd
        for (int col = 1; col < cols - 1; ++col) {
          size_t idx = base + col;
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - stride] + a[idx + stride]) / 8u;
        }
        // last column
        b[base + cols - 1] = a[base + cols - 1];
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}