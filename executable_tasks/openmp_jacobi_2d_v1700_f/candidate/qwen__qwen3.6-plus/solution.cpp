#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

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
  const int C = cols;
  const int R = rows;

  for (int step = 0; step < steps; ++step) {
    std::copy(a.begin(), a.end(), b.begin());

    #pragma omp parallel for schedule(static)
    for (int r = 1; r < R - 1; ++r) {
      const size_t row_off = r * C;
      #pragma omp simd
      for (int c = 1; c < C - 1; ++c) {
        const size_t idx = row_off + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[idx - C] + a[idx + C]) >> 3;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}
