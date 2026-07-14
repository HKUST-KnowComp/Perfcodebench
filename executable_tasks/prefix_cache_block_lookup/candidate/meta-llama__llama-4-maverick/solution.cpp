#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      const int start = request_offsets[r];
      const int end = request_offsets[r + 1];
      int prefix = 0;
      int left = start;
      int right = end;
      while (left < right) {
        const int mid = left + (right - left) / 2;
        if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<std::size_t>(mid)])) {
          left = mid + 1;
          prefix = mid - start + 1;
        } else {
          right = mid;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(prefix));
    }
  }
  return hash;
}
