#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  // Match baseline semantics: if no iterations requested, return 0.
  if (iters <= 0) return 0;

  omp_set_num_threads(num_threads());

  const std::size_t n = input.size();
  const uint32_t* data = n ? input.data() : nullptr;

  uint64_t sum = 0;

  // Compute the sum once using an efficient parallel reduction.
  // This avoids per-element atomics and repeated parallel region overhead.
#pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(data[i]);
  }

  return sum;
}
