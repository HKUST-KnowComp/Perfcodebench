#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  const uint32_t* b = boundaries.data();
  const uint32_t* const b_end = b + boundaries.size();
  const uint32_t* p = probes.data();
  const uint32_t* const p_end = p + probes.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    const uint32_t* cur = b;
    for (const uint32_t* it = p; it != p_end; ++it) {
      const uint32_t probe = *it;
      while (cur != b_end && *cur <= probe) {
        ++cur;
      }
      sum += static_cast<uint64_t>(cur - b);
    }
  }

  return sum;
}
