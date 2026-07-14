#pragma once

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
    // Copy boundary values first (no parallelization needed)
    for (int row = 0; row < rows; ++row) {
      b[row * cols] = a[row * cols];
      b[row * cols + cols - 1] = a[row * cols + cols - 1];
    }
    for (int col = 1; col < cols - 1; ++col) {
      b[col] = a[col];
      b[(rows - 1) * cols + col] = a[(rows - 1) * cols + col];
    }
    
    // Parallelize interior cells only
    #pragma omp parallel for collapse(2) schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = static_cast<std::size_t>(row) * cols + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - cols] + a[idx + cols]) / 8u;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}