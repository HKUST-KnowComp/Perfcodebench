#include "interface.h"

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  (void)iters;
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* k_data = keys.data();
  const uint32_t* v_data = values.data();
  const uint32_t* p_data = probes.data();
  const std::size_t n = keys.size();
  const std::size_t m = probes.size();
  
  for (std::size_t i = 0; i < m; ++i) {
    uint32_t probe = p_data[i];
    std::size_t lo = 0, hi = n;
    while (lo < hi) {
      std::size_t mid = lo + ((hi - lo) >> 1);
      if (k_data[mid] < probe) {
        lo = mid + 1;
      } else {
        hi = mid;
      }
    }
    uint32_t val = v_data[lo];
    hash ^= val;
    hash *= 1099511628211ULL;
  }
  return hash;
}