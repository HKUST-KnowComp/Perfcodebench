#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t probe : probes) {
      size_t low = 0, high = boundaries.size();
      while (low < high) {
        size_t mid = (low + high) / 2;
        if (boundaries[mid] <= probe) {
          low = mid + 1;
        } else {
          high = mid;
        }
      }
      sum += low;
    }
  }
  return sum;
}