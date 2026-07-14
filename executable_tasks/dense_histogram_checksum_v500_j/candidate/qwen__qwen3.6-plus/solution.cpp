#include "interface.h"
#include <vector>
#include <cstring>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;
  const uint32_t* val_ptr = values.data();
  const size_t n = values.size();
  uint32_t* cnt_ptr = counts.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(cnt_ptr, 0, bins * sizeof(uint32_t));
    
    for (size_t i = 0; i < n; ++i) {
      ++cnt_ptr[val_ptr[i]];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash ^= cnt_ptr[b];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}