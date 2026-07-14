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
  std::vector<uint32_t> b(a.size());

  const uint32_t* pa = a.data();
  uint32_t* pb = b.data();

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int r = 0; r < rows; ++r) {
      size_t row_off = static_cast<size_t>(r) * cols;
      if (r == 0 || r == rows - 1) {
        for (int c = 0; c < cols; ++c) {
          pb[row_off + c] = pa[row_off + c];
        }
      } else {
        pb[row_off] = pa[row_off];
        pb[row_off + cols - 1] = pa[row_off + cols - 1];
        for (int c = 1; c < cols - 1; ++c) {
          size_t idx = row_off + c;
          pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] + pa[idx - cols] + pa[idx + cols]) >> 3;
        }
      }
    }
    a.swap(b);
    pa = a.data();
    pb = b.data();
  }

  return checksum_u32(a);
}