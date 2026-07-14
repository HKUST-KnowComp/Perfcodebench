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

  const uint32_t* v = values.data();
  const uint16_t* l = lengths.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      local += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
      local += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
      local += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
      local += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
    }
    for (; i < n; ++i) {
      local += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
    }

    sum = local;
  }
  return sum;
}
