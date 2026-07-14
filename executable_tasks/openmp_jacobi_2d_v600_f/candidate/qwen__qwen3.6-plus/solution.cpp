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
  std::vector<uint32_t> b(a.size());
  const std::size_t stride = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    for (int c = 0; c < cols; ++c) {
      b[c] = a[c];
      b[(rows - 1) * stride + c] = a[(rows - 1) * stride + c];
    }

    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      std::size_t row_start = static_cast<std::size_t>(r) * stride;
      b[row_start] = a[row_start];
      b[row_start + cols - 1] = a[row_start + cols - 1];

      for (int c = 1; c < cols - 1; ++c) {
        std::size_t idx = row_start + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - stride] + a[idx + stride]) >> 3;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}