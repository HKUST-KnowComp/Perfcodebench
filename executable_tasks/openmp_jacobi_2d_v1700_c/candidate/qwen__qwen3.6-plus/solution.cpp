#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>
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

  uint32_t* pa = a.data();
  uint32_t* pb = b.data();

  #pragma omp parallel
  {
    for (int step = 0; step < steps; ++step) {
      #pragma omp single
      {
        std::memcpy(pb, pa, cols * sizeof(uint32_t));
        std::memcpy(pb + (rows - 1) * cols, pa + (rows - 1) * cols, cols * sizeof(uint32_t));
        for (int r = 1; r < rows - 1; ++r) {
          pb[r * cols] = pa[r * cols];
          pb[r * cols + cols - 1] = pa[r * cols + cols - 1];
        }
      }

      #pragma omp for schedule(static)
      for (int r = 1; r < rows - 1; ++r) {
        const uint32_t* __restrict__ row_a = pa + r * cols;
        uint32_t* __restrict__ row_b = pb + r * cols;
        for (int c = 1; c < cols - 1; ++c) {
          uint32_t val = row_a[c];
          uint32_t sum = (val << 2) + row_a[c-1] + row_a[c+1] + row_a[c-cols] + row_a[c+cols];
          row_b[c] = sum >> 3;
        }
      }

      #pragma omp single
      {
        a.swap(b);
        pa = a.data();
        pb = b.data();
      }
    }
  }

  return checksum_u32(a);
}