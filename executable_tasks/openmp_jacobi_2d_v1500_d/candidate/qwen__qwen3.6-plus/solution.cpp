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
  std::vector<uint32_t> b(a.size());

  const int r_end = rows - 1;
  const int c_end = cols - 1;

  for (int step = 0; step < steps; ++step) {
    if (rows > 0) {
      std::memcpy(b.data(), a.data(), cols * sizeof(uint32_t));
      if (rows > 1) {
        std::memcpy(b.data() + r_end * cols, a.data() + r_end * cols, cols * sizeof(uint32_t));
      }
    }
    for (int r = 1; r < r_end; ++r) {
      b[r * cols] = a[r * cols];
      b[r * cols + c_end] = a[r * cols + c_end];
    }

    #pragma omp parallel for schedule(static)
    for (int r = 1; r < r_end; ++r) {
      const uint32_t* a_cur = a.data() + r * cols;
      const uint32_t* a_up  = a.data() + (r - 1) * cols;
      const uint32_t* a_down = a.data() + (r + 1) * cols;
      uint32_t* b_cur = b.data() + r * cols;
      for (int c = 1; c < c_end; ++c) {
        b_cur[c] = (a_cur[c] * 4u + a_cur[c-1] + a_cur[c+1] + a_up[c] + a_down[c]) >> 3;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}