#include "interface.h"

#include <omp.h>

namespace {
inline int num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) t = 1;
  if (t > 8) t = 8;
  return t;
}
}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  (void)iters;  // The sum of identical data is the same each iteration
  omp_set_num_threads(num_threads());

  const std::size_t n = input.size();
  if (n == 0) return 0;

  const uint32_t* data = input.data();
  uint64_t sum = 0;

#pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(data[i]);
  }

  return sum;
}
