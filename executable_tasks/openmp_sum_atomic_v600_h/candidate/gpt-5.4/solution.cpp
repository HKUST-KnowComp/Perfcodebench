#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  if (iters <= 0 || n == 0) return 0;

  const uint32_t* data = input.data();
  uint64_t sum = 0;

#pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local += static_cast<uint64_t>(data[i]);
      }
#pragma omp atomic
      sum += local;
#pragma omp barrier
#pragma omp single
      {
        if (iter + 1 != iters) sum = 0;
      }
#pragma omp barrier
    }
  }

  return sum;
}
