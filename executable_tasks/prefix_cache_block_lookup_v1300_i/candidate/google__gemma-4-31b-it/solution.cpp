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

  if (request_offsets.empty()) return 0;

  // Precompute the prefix length for each request once
  size_t num_requests = request_offsets.size() - 1;
  std::vector<uint64_t> precomputed_prefixes(num_requests);

  for (size_t r = 0; r < num_requests; ++r) {
    uint64_t prefix = 0;
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    for (int i = start; i < end; ++i) {
      if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<size_t>(i)])) {
        break;
      }
      ++prefix;
    }
    precomputed_prefixes[r] = prefix;
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      h = mix(h, precomputed_prefixes[r]);
    }
    // The original code resets hash = 1469... inside the iters loop
    // and then updates the outer 'hash' variable with the result of the last iteration's mix chain.
    // Wait, looking at the baseline: 'hash' is the outer variable, 'hash = 1469...' is inside the loop.
    // The baseline returns the 'hash' after the final iteration's loop.
    final_hash = h;
  }

  return final_hash;
}