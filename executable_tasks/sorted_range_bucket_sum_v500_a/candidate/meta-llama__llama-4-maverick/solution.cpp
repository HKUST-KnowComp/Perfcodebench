#include "interface.h"

uint64_t binary_search(const std::vector<uint32_t>& boundaries, uint32_t probe) {
  int low = 0;
  int high = boundaries.size();
  while (low < high) {
    int mid = low + (high - low) / 2;
    if (boundaries[mid] <= probe) {
      low = mid + 1;
    } else {
      high = mid;
    }
  }
  return low;
}

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t probe : probes) {
      sum += binary_search(boundaries, probe);
    }
  }
  return sum;
}