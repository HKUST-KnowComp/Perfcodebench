#include "interface.h"

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
  // Build O(1) lookup table for cache membership
  std::vector<char> cache_present(hash_space, 0);
  for (uint32_t key : sorted_cache_keys) {
    cache_present[key] = 1;
  }

  uint64_t hash = 0;
  const std::size_t num_requests = request_offsets.size() - 1;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      int i = start;
      for (; i < end; ++i) {
        if (!cache_present[request_blocks[i]]) {
          break;
        }
      }
      uint64_t prefix = static_cast<uint64_t>(i - start);
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
