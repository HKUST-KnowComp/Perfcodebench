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
  uint64_t total_sum = 0;
  const size_t n = input.size();
  const uint32_t* data = input.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    #pragma omp parallel for reduction(+:current_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      current_sum += static_cast<uint64_t>(data[i]);
    }
    total_sum = current_sum;
  }
  return total_sum;
}