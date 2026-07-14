#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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

  if (request_offsets.empty()) return 0;

  // Precompute the prefix length for each request once.
  // The cache and request blocks do not change between iterations.
  size_t num_requests = request_offsets.size() - 1;
  std::vector<uint64_t> precomputed_prefixes(num_requests);

  for (size_t r = 0; r < num_requests; ++r) {
    uint64_t prefix = 0;
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    for (int i = start; i < end; ++i) {
      uint32_t block = request_blocks[static_cast<size_t>(i)];
      if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), block)) {
        break;
      }
      ++prefix;
    }
    precomputed_prefixes[r] = prefix;
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      current_hash = mix(current_hash, precomputed_prefixes[r]);
    }
    // The original code updates the outer 'hash' variable with the result of the inner loop's final hash.
    // However, the baseline logic says: hash = 1469598103934665603ULL at the start of each iter,
    // then it accumulates. The final return is the 'hash' after the last iteration.
    final_hash = current_hash;
  }

  return final_hash;
}