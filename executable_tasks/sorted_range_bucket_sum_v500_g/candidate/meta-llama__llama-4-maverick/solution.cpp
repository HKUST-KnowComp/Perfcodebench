#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto probes_begin = probes.begin();
    auto boundaries_begin = boundaries.begin();
    auto boundaries_end = boundaries.end();
    for (; probes_begin != probes.end(); ++probes_begin) {
      auto it = std::upper_bound(boundaries_begin, boundaries_end, *probes_begin);
      sum += static_cast<uint64_t>(it - boundaries_begin);
      boundaries_begin = it;
    }
  }
  return sum;
}