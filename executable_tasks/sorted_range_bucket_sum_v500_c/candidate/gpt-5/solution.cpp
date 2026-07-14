#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Match baseline behavior: if iters <= 0, return 0
  if (iters <= 0) return 0;

  // If no probes, sum is zero
  if (probes.empty()) return 0;

  // If there are no boundaries, every bucket index is 0
  if (boundaries.empty()) return 0;

  const uint32_t* b = boundaries.data();
  const uint32_t* be = b + boundaries.size();
  const uint32_t* bstart = b;

  const uint32_t* p = probes.data();
  const uint32_t* pe = p + probes.size();

  uint64_t sum = 0;

  // Two-pointer sweep: for each probe, advance boundary pointer while boundary <= probe
  while (p < pe) {
    uint32_t v = *p++;
    while (b < be && *b <= v) {
      ++b;
    }
    sum += static_cast<uint64_t>(b - bstart);
  }

  return sum;
}
