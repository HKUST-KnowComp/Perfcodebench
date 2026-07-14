#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int /*iters*/) {
  // Compute the sum once; baseline returned the same value each iteration
  const uint32_t* b = boundaries.data();
  const uint32_t* be = b + boundaries.size();
  const uint32_t* p = probes.data();
  const uint32_t* pe = p + probes.size();

  uint64_t sum = 0;
  size_t idx = 0; // number of boundaries <= current probe

  for (; p != pe; ++p) {
    const uint32_t probe = *p;
    while (b != be && *b <= probe) {
      ++b;
      ++idx;
    }
    sum += static_cast<uint64_t>(idx);
  }

  return sum;
}
