#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* b = boundaries.data();
  const uint32_t* bend = b + boundaries.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* cur = b; // current position in boundaries
    const uint32_t* p = probes.data();
    const uint32_t* pend = p + probes.size();

    while (p != pend) {
      const uint32_t v = *p;
      while (cur != bend && *cur <= v) {
        ++cur;
      }
      sum += static_cast<uint64_t>(cur - b);
      ++p;
    }
  }

  return sum;
}
