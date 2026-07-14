#include "interface.h"
#include <vector>
#include <algorithm>

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
  
  const size_t num_requests = request_offsets.empty() ? 0 : request_offsets.size() - 1;
  std::vector<uint64_t> prefixes(num_requests);
  
  for (size_t r = 0; r < num_requests; ++r) {
    uint64_t prefix = 0;
    const int start = request_offsets[r];
    const int end = request_offsets[r + 1];
    for (int i = start; i < end; ++i) {
      if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<size_t>(i)])) {
        break;
      }
      ++prefix;
    }
    prefixes[r] = prefix;
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t p : prefixes) {
      hash = mix(hash, p);
    }
  }
  return hash;
}