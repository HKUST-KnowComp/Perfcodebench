#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* __restrict__ data = input.data();
  const std::size_t n = input.size();
  const std::size_t n4 = n & ~std::size_t(3);
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
#pragma omp parallel for reduction(+:local_sum) schedule(static)
    for (std::size_t i = 0; i < n4; i += 4) {
      local_sum += static_cast<uint64_t>(data[i]) +
                   static_cast<uint64_t>(data[i + 1]) +
                   static_cast<uint64_t>(data[i + 2]) +
                   static_cast<uint64_t>(data[i + 3]);
    }
    for (std::size_t i = n4; i < n; ++i) {
      local_sum += static_cast<uint64_t>(data[i]);
    }
    sum = local_sum;
  }
  return sum;
}