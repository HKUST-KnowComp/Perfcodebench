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
  std::vector<uint32_t> b = a; // Boundary values initialized once, never modified
  const std::size_t rows_sz = static_cast<std::size_t>(rows);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static) default(none) shared(a, b, rows_sz, cols_sz)
    for (std::size_t row = 1; row < rows_sz - 1; ++row) {
      #pragma omp simd
      for (std::size_t col = 1; col < cols_sz - 1; ++col) {
        const std::size_t idx = row * cols_sz + col;
        b[idx] = (4u * a[idx] + a[idx - 1] + a[idx + 1] + a[idx - cols_sz] + a[idx + cols_sz]) / 8u;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}