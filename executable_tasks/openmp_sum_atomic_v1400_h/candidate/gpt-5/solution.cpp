#include "interface.h"
#include <omp.h>

namespace {
inline int clamp_threads(int max_threads) {
  if (max_threads < 1) return 1;
  if (max_threads > 8) return 8;
  return max_threads;
}
}

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  if (iters <= 0 || input.empty()) {
    return 0ull;
  }

  omp_set_num_threads(clamp_threads(omp_get_max_threads()));

  const uint32_t* data = input.data();
  const std::size_t n = input.size();

  uint64_t total = 0ull;
#pragma omp parallel for reduction(+:total) schedule(static)
  for (std::size_t i = 0; i < n; ++i) {
    total += static_cast<uint64_t>(data[i]);
  }

  uint64_t iters64 = static_cast<uint64_t>(iters);
  return total * iters64; // wraps modulo 2^64, matching repeated addition semantics
}
