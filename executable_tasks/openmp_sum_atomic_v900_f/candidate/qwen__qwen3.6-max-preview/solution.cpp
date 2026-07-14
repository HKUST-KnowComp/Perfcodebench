#include "interface.h"
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t sum = 0;
  const uint32_t* data = input.data();
  const std::size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
#pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += data[i];
    }
    sum = iter_sum;
  }
  return sum;
}
