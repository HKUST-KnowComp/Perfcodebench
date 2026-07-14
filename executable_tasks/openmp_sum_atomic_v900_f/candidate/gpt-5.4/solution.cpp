#include "interface.h"

#include <omp.h>

#include <cstdint>
#include <vector>

namespace {

inline int choose_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  if (iters <= 0 || n == 0) return 0;

  omp_set_num_threads(choose_threads());

  const uint32_t* data = input.data();
  uint64_t sum = 0;

#pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
#pragma omp single
      sum = 0;

      uint64_t local = 0;
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local += static_cast<uint64_t>(data[i]);
      }

#pragma omp atomic
      sum += local;

#pragma omp barrier
    }
  }

  return sum;
}
