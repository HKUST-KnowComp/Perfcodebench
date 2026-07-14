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
  (void)hash_space;
  const uint32_t* cache = sorted_cache_keys.data();
  const std::size_t cache_size = sorted_cache_keys.size();
  const uint32_t* blocks = request_blocks.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t cache_idx = 0;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      const int start = request_offsets[r];
      const int end   = request_offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        const uint32_t key = blocks[static_cast<std::size_t>(i)];
        while (cache_idx < cache_size && cache[cache_idx] < key) {
          ++cache_idx;
        }
        if (cache_idx >= cache_size || cache[cache_idx] != key) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}