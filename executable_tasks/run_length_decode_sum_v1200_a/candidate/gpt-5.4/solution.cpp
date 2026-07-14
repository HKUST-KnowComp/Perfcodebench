#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  const std::size_t n = values.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;

    const uint32_t* vptr = values.data();
    const uint16_t* lptr = lengths.data();
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(vptr[i]) * static_cast<uint64_t>(lptr[i]);
    }

    sum = iter_sum;
  }
  return sum;
}
