#include "interface.h"

#include <omp.h>

namespace {

inline int choose_num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
}

inline void ensure_thread_count_set() {
  static bool initialized = false;
  if (!initialized) {
    omp_set_num_threads(choose_num_threads());
    initialized = true;
  }
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  ensure_thread_count_set();

  const std::size_t n = input.size();
  const uint32_t* data = input.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
#pragma omp parallel for reduction(+ : iter_sum) schedule(static)
    for (long long i = 0; i < static_cast<long long>(n); ++i) {
      iter_sum += static_cast<uint64_t>(data[i]);
    }
    sum = iter_sum;
  }

  return sum;
}
