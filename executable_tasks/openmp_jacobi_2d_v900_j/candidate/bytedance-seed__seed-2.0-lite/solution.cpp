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
  const size_t rows_s = static_cast<size_t>(rows);
  const size_t cols_s = static_cast<size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (size_t i = 1; i < rows_s - 1; ++i) {
      const size_t row_offset = i * cols_s;
      const size_t prev_row_offset = row_offset - cols_s;
      const size_t next_row_offset = row_offset + cols_s;
      
      uint32_t* __restrict__ b_row = &b[row_offset];
      const uint32_t* __restrict__ a_curr = &a[row_offset];
      const uint32_t* __restrict__ a_prev = &a[prev_row_offset];
      const uint32_t* __restrict__ a_next = &a[next_row_offset];
      
      for (size_t j = 1; j < cols_s - 1; ++j) {
        const uint32_t center = a_curr[j] * 4u;
        const uint32_t left = a_curr[j-1];
        const uint32_t right = a_curr[j+1];
        const uint32_t up = a_prev[j];
        const uint32_t down = a_next[j];
        b_row[j] = (center + left + right + up + down) / 8u;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}