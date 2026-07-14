#include "interface.h"
#include <omp.h>

#include <cstddef>
#include <cstdint>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int /*iters*/) {
  const std::size_t n = input.size();
  if (n == 0) return 0;

  const uint32_t* data = input.data();

  // For small sizes or single-threaded environments, use a fast serial path.
  int max_threads = omp_get_max_threads();
  if (max_threads <= 1 || n < (1u << 15)) {
    uint64_t s = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      s += data[i + 0];
      s += data[i + 1];
      s += data[i + 2];
      s += data[i + 3];
    }
    for (; i < n; ++i) {
      s += data[i];
    }
    return s;
  }

  uint64_t sum = 0;
#pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(data[i]);
  }
  return sum;
}
