#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int /*iters*/) {
  const uint32_t* b = boundaries.data();
  const uint32_t* be = b + boundaries.size();
  const uint32_t* p = probes.data();
  const uint32_t* pe = p + probes.size();

  const uint32_t* bp = b;
  uint64_t sum = 0;

  while (p != pe) {
    uint32_t probe = *p++;
    while (bp != be && *bp <= probe) {
      ++bp;
    }
    size_t idx = static_cast<size_t>(bp - b);
    if (bp == be) {
      sum += idx;                  // current probe
      sum += idx * static_cast<uint64_t>(pe - p); // all remaining probes
      break;
    }
    sum += idx;
  }

  return sum;
}
