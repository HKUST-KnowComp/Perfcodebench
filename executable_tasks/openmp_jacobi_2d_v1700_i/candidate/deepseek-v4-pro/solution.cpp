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
  const std::size_t n = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(n, 0);

  const std::size_t stride = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    for (std::size_t col = 0; col < stride; ++col) {
      b[col] = a[col];
    }
    if (rows > 1) {
      std::size_t bottom_start = static_cast<std::size_t>(rows - 1) * stride;
      for (std::size_t col = 0; col < stride; ++col) {
        b[bottom_start + col] = a[bottom_start + col];
      }
    }

    // Copy left and right boundaries for interior rows
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t idx_left = static_cast<std::size_t>(row) * stride;
      std::size_t idx_right = idx_left + stride - 1;
      b[idx_left] = a[idx_left];
      b[idx_right] = a[idx_right];
    }

    // Interior points
    if (rows > 2 && cols > 2) {
      #pragma omp parallel for schedule(static)
      for (int row = 1; row < rows - 1; ++row) {
        std::size_t base = static_cast<std::size_t>(row) * stride;
        #pragma omp simd
        for (int col = 1; col < cols - 1; ++col) {
          std::size_t idx = base + static_cast<std::size_t>(col);
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - stride] + a[idx + stride]) /
                   8u;
        }
      }
    }

    std::swap(a, b); // a becomes current result, b will be reused
  }
  return checksum_u32(a);
}
