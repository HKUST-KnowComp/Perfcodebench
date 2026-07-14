#include "interface.h"

#include <omp.h>

namespace {

int num_threads() {
  int mt = omp_get_max_threads();
  if (mt < 1) mt = 1;
  if (mt > 8) mt = 8;
  return mt;
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  omp_set_num_threads(num_threads());

  const uint32_t* data = input.data();
  const std::size_t n = input.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
#pragma omp parallel for schedule(static) reduction(+:s)
    for (std::size_t i = 0; i < n; ++i) {
      s += static_cast<uint64_t>(data[i]);
    }
    sum = s;
  }
  return sum;
}
