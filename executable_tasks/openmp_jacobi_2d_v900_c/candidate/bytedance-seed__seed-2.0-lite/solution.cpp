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
  const size_t col_stride = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static) default(none) shared(a,b,rows,cols,col_stride)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_base = static_cast<size_t>(row) * col_stride;
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_base + static_cast<size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - col_stride] + a[idx + col_stride]) / 8u;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}