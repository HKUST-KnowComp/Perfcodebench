#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* const data = input.data();
  const int64_t n = static_cast<int64_t>(input.size());
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
#pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (int64_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(data[i]);
    }
    sum = iter_sum;
  }
  return sum;
}
