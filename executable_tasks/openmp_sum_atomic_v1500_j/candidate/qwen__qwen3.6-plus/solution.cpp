#include "interface.h"
#include <algorithm>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t sum = 0;
  const size_t n = input.size();
  const uint32_t* data = input.data();
  
  omp_set_num_threads(std::max(1, std::min(8, omp_get_max_threads())));

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (size_t i = 0; i < n; ++i) {
      sum += data[i];
    }
  }
  return sum;
}