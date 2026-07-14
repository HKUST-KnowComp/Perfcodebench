#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  if (iters <= 0 || n == 0) {
    return 0ull;
  }

  omp_set_dynamic(0);
  omp_set_num_threads(num_threads());

  uint64_t sum = 0;
  const uint32_t* data = input.data();

#pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
#pragma omp single
      {
        sum = 0ull;
      }
#pragma omp for schedule(static) reduction(+:sum)
      for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(data[i]);
      }
    }
  }

  return sum;
}
