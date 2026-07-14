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

  const size_t cols_sz = static_cast<size_t>(cols);

  #pragma omp parallel
  {
    for (int step = 0; step < steps; ++step) {
      #pragma omp single
      {
        std::memcpy(b.data(), a.data(), cols_sz * sizeof(uint32_t));
        std::memcpy(b.data() + (rows - 1) * cols_sz, a.data() + (rows - 1) * cols_sz, cols_sz * sizeof(uint32_t));
        for (int r = 1; r < rows - 1; ++r) {
          b[r * cols_sz] = a[r * cols_sz];
          b[r * cols_sz + cols_sz - 1] = a[r * cols_sz + cols_sz - 1];
        }
      }

      const uint32_t* __restrict__ pa = a.data();
      uint32_t* __restrict__ pb = b.data();

      #pragma omp for schedule(static)
      for (int r = 1; r < rows - 1; ++r) {
        size_t i = r * cols_sz + 1;
        const size_t i_end = i + cols_sz - 2;
        for (; i < i_end; ++i) {
          pb[i] = (pa[i] * 4u + pa[i - 1] + pa[i + 1] + pa[i - cols_sz] + pa[i + cols_sz]) >> 3;
        }
      }

      #pragma omp single
      a.swap(b);
    }
  }

  return checksum_u32(a);
}
