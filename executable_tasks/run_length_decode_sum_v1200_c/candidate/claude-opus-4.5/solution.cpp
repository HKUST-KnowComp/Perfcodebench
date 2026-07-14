#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  const uint32_t* __restrict vptr = values.data();
  const uint16_t* __restrict lptr = lengths.data();
  const std::size_t n = values.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      s0 += static_cast<uint64_t>(vptr[i])     * lptr[i];
      s1 += static_cast<uint64_t>(vptr[i + 1]) * lptr[i + 1];
      s2 += static_cast<uint64_t>(vptr[i + 2]) * lptr[i + 2];
      s3 += static_cast<uint64_t>(vptr[i + 3]) * lptr[i + 3];
    }
    for (; i < n; ++i) {
      s0 += static_cast<uint64_t>(vptr[i]) * lptr[i];
    }
    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}
