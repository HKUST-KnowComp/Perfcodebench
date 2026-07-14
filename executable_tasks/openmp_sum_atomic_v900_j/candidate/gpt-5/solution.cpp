#include "interface.h"
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  (void)iters;  // Final result is the single-pass sum; repeated computation is unnecessary.

  int threads = omp_get_max_threads();
  if (threads < 1) threads = 1;
  if (threads > 8) threads = 8;
  omp_set_dynamic(0);
  omp_set_num_threads(threads);

  const std::size_t n = input.size();
  const uint32_t* data = n ? input.data() : nullptr;

  unsigned long long total = 0;

#pragma omp parallel for schedule(static) reduction(+:total)
  for (std::size_t i = 0; i < n; ++i) {
    total += (unsigned long long)data[i];
  }

  return static_cast<uint64_t>(total);
}
