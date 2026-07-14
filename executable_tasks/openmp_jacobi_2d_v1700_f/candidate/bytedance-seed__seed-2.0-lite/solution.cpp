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
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  const std::size_t total_sz = static_cast<std::size_t>(rows) * cols_sz;
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = a;

  // Boundaries are immutable, so no need to update them in every step
  if (steps <= 0) {
    return checksum_u32(a);
  }

  const int inner_row_start = 1;
  const int inner_row_end = rows - 2;
  const int inner_col_start = 1;
  const int inner_col_end = cols - 2;

  for (int step = 0; step < steps; ++step) {
    if (inner_row_start > inner_row_end || inner_col_start > inner_col_end) {
      // All cells are boundaries, no updates needed
      a.swap(b);
      continue;
    }

    #pragma omp parallel for schedule(static) default(none) shared(a, b, cols_sz) firstprivate(inner_row_end, inner_col_start, inner_col_end)
    for (int row = inner_row_start; row <= inner_row_end; ++row) {
      const std::size_t row_base = static_cast<std::size_t>(row) * cols_sz;
      #pragma omp simd
      for (int col = inner_col_start; col <= inner_col_end; ++col) {
        const std::size_t idx = row_base + static_cast<std::size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - cols_sz] + a[idx + cols_sz]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}