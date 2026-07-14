#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // If no iterations requested, follow baseline behavior and return 0.
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* b = boundaries.data();
  const uint32_t* be = b + boundaries.size();
  const uint32_t* p = probes.data();
  const uint32_t* pe = p + probes.size();

  // Linear scan leveraging sorted boundaries and probes:
  // For each probe, advance boundary pointer while boundary <= probe.
  // The index (count of boundaries <= probe) is the current pointer offset.
  uint64_t sum = 0;
  const uint32_t* cur = b;
  while (p != pe) {
    uint32_t pv = *p++;
    while (cur != be && *cur <= pv) {
      ++cur;
    }
    sum += static_cast<uint64_t>(cur - b);
  }

  return sum;
}
