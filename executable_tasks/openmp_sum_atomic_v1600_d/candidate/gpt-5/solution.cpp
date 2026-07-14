#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = input.size();
  if (n == 0) return 0;

  omp_set_num_threads(num_threads());

  const uint32_t* data = input.data();
  uint64_t total = 0;

#pragma omp parallel
  {
    uint64_t local = 0;
    int tid = omp_get_thread_num();
    int nt = omp_get_num_threads();

    std::size_t start = (n * static_cast<std::size_t>(tid)) / static_cast<std::size_t>(nt);
    std::size_t end = (n * static_cast<std::size_t>(tid + 1)) / static_cast<std::size_t>(nt);

    // Unrolled accumulation to increase ILP
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = start;
    std::size_t limit = start + ((end - start) & ~static_cast<std::size_t>(7));
    for (; i < limit; i += 8) {
      s0 += data[i + 0];
      s1 += data[i + 1];
      s2 += data[i + 2];
      s3 += data[i + 3];
      s0 += data[i + 4];
      s1 += data[i + 5];
      s2 += data[i + 6];
      s3 += data[i + 7];
    }
    local = s0 + s1 + s2 + s3;

    for (; i < end; ++i) {
      local += data[i];
    }

#pragma omp atomic
    total += local;
  }

  return total;
}
