#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;

  std::vector<uint32_t> sorted_probes = probes;
  std::sort(sorted_probes.begin(), sorted_probes.end());

  uint64_t sum = 0;
  auto it = boundaries.cbegin();
  const auto end = boundaries.cend();

  for (uint32_t p : sorted_probes) {
    it = std::upper_bound(it, end, p);
    sum += static_cast<uint64_t>(it - boundaries.cbegin());
  }

  return sum;
}