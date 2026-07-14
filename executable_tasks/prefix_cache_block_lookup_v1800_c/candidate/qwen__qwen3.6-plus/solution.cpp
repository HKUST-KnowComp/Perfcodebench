#include "interface.h"

#include <algorithm>
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
  
  const uint32_t* cache_data = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_data + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  const int* offsets = request_offsets.data();
  
  std::vector<uint64_t> lengths;
  lengths.reserve(request_offsets.size() > 0 ? request_offsets.size() - 1 : 0);
  
  for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
    uint64_t prefix = 0;
    int start = offsets[r];
    int end = offsets[r + 1];
    for (int i = start; i < end; ++i) {
      if (!std::binary_search(cache_data, cache_end, blocks_data[i])) {
        break;
      }
      ++prefix;
    }
    lengths.push_back(prefix);
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t len : lengths) {
      hash = mix(hash, len);
    }
  }
  return hash;
}
