#include "interface.h"
#include <omp.h>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t result = 0;
  int nthreads = std::max(1, std::min(8, omp_get_max_threads()));
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    const uint32_t* data = input.data();
    std::size_t n = input.size();
    #pragma omp parallel for num_threads(nthreads) reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      sum += data[i];
    }
    result = sum;
  }
  return result;
}
