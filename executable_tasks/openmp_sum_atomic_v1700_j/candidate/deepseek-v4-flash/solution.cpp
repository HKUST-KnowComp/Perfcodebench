#include "interface.h"
#include <omp.h>

namespace {
int num_threads() {
  int max_th = omp_get_max_threads();
  return (max_th > 8) ? 8 : (max_th < 1 ? 1 : max_th);
}
}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  omp_set_num_threads(num_threads());
  uint64_t sum = 0;
  const uint32_t* data = input.data();
  const size_t n = input.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(data[i]);
    }
  }
  return sum;
}
