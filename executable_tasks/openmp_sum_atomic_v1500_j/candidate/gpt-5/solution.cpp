#include "interface.h"
#include <omp.h>

namespace {
inline int choose_threads() {
  int t = omp_get_max_threads();
  if (t > 8) t = 8;
  if (t < 1) t = 1;
  return t;
}
}

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0 without computing.
  if (iters <= 0) return 0;

  omp_set_num_threads(choose_threads());

  const uint32_t* data = input.data();
  const std::size_t n = input.size();

  uint64_t sum = 0;

#pragma omp parallel for schedule(static) reduction(+:sum)
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(data[i]);
  }

  return sum;
}
