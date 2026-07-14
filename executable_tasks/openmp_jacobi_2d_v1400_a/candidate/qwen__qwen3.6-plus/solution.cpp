#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>
#include <cstring>

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

  const size_t cols_sz = static_cast<size_t>(cols);
  const size_t rows_sz = static_cast<size_t>(rows);

  for (int step = 0; step < steps; ++step) {
    uint32_t* pa = a.data();
    uint32_t* pb = b.data();

    // Copy top and bottom boundary rows
    std::memcpy(pb, pa, cols_sz * sizeof(uint32_t));
    if (rows_sz > 1) {
      std::memcpy(pb + (rows_sz - 1) * cols_sz, pa + (rows_sz - 1) * cols_sz, cols_sz * sizeof(uint32_t));
    }

    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      size_t base = r * cols_sz;
      // Copy left and right boundary columns
      pb[base] = pa[base];
      pb[base + cols_sz - 1] = pa[base + cols_sz - 1];

      // Compute interior
      #pragma omp simd
      for (int c = 1; c < cols - 1; ++c) {
        size_t idx = base + c;
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] + pa[idx - cols_sz] + pa[idx + cols_sz]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
