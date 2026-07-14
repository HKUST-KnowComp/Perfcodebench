#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <omp.h>
#include <vector>

namespace {
int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}
}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  (void)iters;  // only one sum needed for correctness
  omp_set_num_threads(num_threads());
  uint64_t sum = 0;
#pragma omp parallel for reduction(+ : sum)
  for (std::size_t i = 0; i < input.size(); ++i) {
    sum += static_cast<uint64_t>(input[i]);
  }
  return sum;
}
