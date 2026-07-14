#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  omp_set_num_threads(num_threads());
  uint64_t sum = 0;
  const std::size_t input_size = input.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
#pragma omp parallel for schedule(static) reduction(+:sum)
    for (std::size_t i = 0; i < input_size; ++i) {
      sum += static_cast<uint64_t>(input[i]);
    }
  }
  return sum;
}