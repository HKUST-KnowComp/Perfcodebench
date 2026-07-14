#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (iters <= 0 || n == 0) return 0;

  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    uint32_t prev = 0;

    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      const uint32_t re0 = re[i];
      const uint32_t re1 = re[i + 1];
      const uint32_t re2 = re[i + 2];
      const uint32_t re3 = re[i + 3];

      local_sum += static_cast<uint64_t>(re0 - prev) * static_cast<uint64_t>(val[i]);
      local_sum += static_cast<uint64_t>(re1 - re0) * static_cast<uint64_t>(val[i + 1]);
      local_sum += static_cast<uint64_t>(re2 - re1) * static_cast<uint64_t>(val[i + 2]);
      local_sum += static_cast<uint64_t>(re3 - re2) * static_cast<uint64_t>(val[i + 3]);

      prev = re3;
    }

    for (; i < n; ++i) {
      const uint32_t end = re[i];
      local_sum += static_cast<uint64_t>(end - prev) * static_cast<uint64_t>(val[i]);
      prev = end;
    }

    sum = local_sum;
  }
  return sum;
}
