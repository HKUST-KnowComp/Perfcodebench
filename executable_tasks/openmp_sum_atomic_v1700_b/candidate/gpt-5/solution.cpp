#include "interface.h"

#include <omp.h>

namespace {

int num_threads() {
  // Cap threads to avoid overhead on very high core counts
  int mt = omp_get_max_threads();
  if (mt < 1) mt = 1;
  if (mt > 8) mt = 8;
  return mt;
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  const uint32_t* d = n ? input.data() : nullptr;

  omp_set_dynamic(0);
  omp_set_num_threads(num_threads());

  uint64_t result = 0;
  // Handle empty input quickly
  if (n == 0) {
    return 0ull;
  }

  // Compute the sum iters times as required
  for (int iter = 0; iter < iters; ++iter) {
    unsigned long long total = 0ull;

    // Main body: process in chunks of 8 for better ILP
    const std::size_t main_end = n & ~static_cast<std::size_t>(7);
#pragma omp parallel for schedule(static) reduction(+:total)
    for (std::size_t i = 0; i < main_end; i += 8) {
      total += static_cast<unsigned long long>(d[i])
             + static_cast<unsigned long long>(d[i + 1])
             + static_cast<unsigned long long>(d[i + 2])
             + static_cast<unsigned long long>(d[i + 3])
             + static_cast<unsigned long long>(d[i + 4])
             + static_cast<unsigned long long>(d[i + 5])
             + static_cast<unsigned long long>(d[i + 6])
             + static_cast<unsigned long long>(d[i + 7]);
    }

    // Remainder
    for (std::size_t i = main_end; i < n; ++i) {
      total += static_cast<unsigned long long>(d[i]);
    }

    result = static_cast<uint64_t>(total);
  }

  return result;
}
