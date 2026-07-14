#include "interface.h"
#include <cstddef>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

  const uint32_t* v = values.data();
  const size_t n = values.size();
  const uint32_t* mn = mins.data();
  const uint32_t* mx = maxs.data();
  const size_t num_blocks = (n + block_size - 1) / block_size;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t idx = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      const uint32_t block_min = mn[b];
      const uint32_t block_max = mx[b];
      if (block_max < low || block_min > high) {
        idx += block_size;
        continue;
      }
      const size_t block_end = (idx + block_size < n) ? idx + block_size : n;
      const uint32_t* ptr = v + idx;
      const size_t len = block_end - idx;
      const uint32_t* end = ptr + len;

      uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
      const uint32_t* e8 = ptr + (len & ~size_t(7));
      for (; ptr < e8; ptr += 8) {
        uint32_t x0 = ptr[0], x1 = ptr[1], x2 = ptr[2], x3 = ptr[3];
        uint32_t x4 = ptr[4], x5 = ptr[5], x6 = ptr[6], x7 = ptr[7];
        if (x0 >= low && x0 <= high) s0 += x0;
        if (x1 >= low && x1 <= high) s1 += x1;
        if (x2 >= low && x2 <= high) s2 += x2;
        if (x3 >= low && x3 <= high) s3 += x3;
        if (x4 >= low && x4 <= high) s4 += x4;
        if (x5 >= low && x5 <= high) s5 += x5;
        if (x6 >= low && x6 <= high) s6 += x6;
        if (x7 >= low && x7 <= high) s7 += x7;
      }
      for (; ptr < end; ++ptr) {
        uint32_t x = *ptr;
        if (x >= low && x <= high) s0 += x;
      }
      sum += s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
      idx = block_end;
    }
  }
  return sum;
}