#include "interface.h"
#include <omp.h>

namespace {
  int num_threads() {
    int t = omp_get_max_threads();
    if (t < 1) return 1;
    if (t > 8) return 8;
    return t;
  }
}

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  omp_set_num_threads(num_threads());
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    #pragma omp parallel for schedule(static) reduction(+:sum)
    for (std::size_t i = 0; i < input.size(); ++i) {
      sum += static_cast<uint64_t>(input[i]);
    }
  }
  return sum;
}
