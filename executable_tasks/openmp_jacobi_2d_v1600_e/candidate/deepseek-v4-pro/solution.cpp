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
  const std::size_t stride = static_cast<std::size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
#pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const std::size_t idx_base = static_cast<std::size_t>(row) * stride;
      if (row == 0 || row == rows - 1) {
        // Top or bottom row: just copy
        for (int col = 0; col < cols; ++col) {
          b[idx_base + col] = a[idx_base + col];
        }
      } else {
        // Left boundary
        b[idx_base + 0] = a[idx_base + 0];
        // Right boundary
        b[idx_base + cols - 1] = a[idx_base + cols - 1];
        
        // Interior columns - vectorizable with no branches
#pragma omp simd
        for (int col = 1; col < cols - 1; ++col) {
          const std::size_t idx = idx_base + col;
          const uint32_t sum = a[idx] * 4u +
                               a[idx - 1] +
                               a[idx + 1] +
                               a[idx - stride] +
                               a[idx + stride];
          b[idx] = sum / 8u;
        }
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}