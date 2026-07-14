#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  const uint32_t* cache_data = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_data + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  const int* offsets_data = request_offsets.data();
  size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_data[r];
      int end = offsets_data[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (!std::binary_search(cache_data, cache_end, blocks_data[i])) {
          break;
        }
        ++prefix;
      }
      hash ^= prefix;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}