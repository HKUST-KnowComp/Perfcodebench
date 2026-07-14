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
  if (rows <= 2 || cols <= 2 || steps == 0) {
    return checksum_u32(input);
  }

  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = a;
  const size_t s_cols = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static) shared(a, b, s_cols, rows, cols) default(none)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_base = static_cast<size_t>(row) * s_cols;
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_base + static_cast<size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[idx - s_cols] + a[idx + s_cols]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}