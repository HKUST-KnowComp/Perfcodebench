#include "interface.h"

#include <omp.h>

#include <cstdint>
#include <vector>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  if (iters <= 0 || n == 0) return 0;

  uint64_t final_sum = 0;

#pragma omp parallel
  {
    uint64_t local_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t iter_local = 0;

#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        iter_local += static_cast<uint64_t>(input[i]);
      }

#pragma omp single
      final_sum = 0;

      local_sum = iter_local;

#pragma omp barrier
#pragma omp atomic
      final_sum += local_sum;
#pragma omp barrier
    }
  }

  return final_sum;
}
