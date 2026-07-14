#include "interface.h"

#include <omp.h>

namespace {

inline int chosen_num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
}

inline void ensure_num_threads_set() {
  static const int once = []() {
    omp_set_num_threads(chosen_num_threads());
    return 0;
  }();
  (void)once;
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  ensure_num_threads_set();

  const std::size_t n = input.size();
  const uint32_t* data = n ? input.data() : nullptr;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t total = 0;

#pragma omp parallel for schedule(static) reduction(+:total)
    for (std::size_t i = 0; i < n; i += 4) {
      uint64_t local = static_cast<uint64_t>(data[i]);
      if (i + 1 < n) local += static_cast<uint64_t>(data[i + 1]);
      if (i + 2 < n) local += static_cast<uint64_t>(data[i + 2]);
      if (i + 3 < n) local += static_cast<uint64_t>(data[i + 3]);
      total += local;
    }

    sum = total;
  }
  return sum;
}
