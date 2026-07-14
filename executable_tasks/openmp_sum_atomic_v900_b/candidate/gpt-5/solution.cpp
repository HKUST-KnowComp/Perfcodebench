#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int /*iters*/) {
  const std::size_t n = input.size();
  if (n == 0) return 0;

  const uint32_t* data = input.data();
  unsigned long long total = 0ULL;

#pragma omp parallel for schedule(static) reduction(+:total)
  for (std::size_t i = 0; i < n; ++i) {
    total += static_cast<unsigned long long>(data[i]);
  }

  return static_cast<uint64_t>(total);
}
