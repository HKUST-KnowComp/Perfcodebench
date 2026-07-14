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
  const uint32_t* data = input.data();
  const std::size_t n = input.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(data[i]);
    }
  }
  return sum;
}
