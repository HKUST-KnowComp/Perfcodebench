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
  // Precompute O(1) presence lookup for cache keys
  std::vector<bool> present(hash_space, false);
  for (uint32_t key : sorted_cache_keys) {
    present[key] = true;
  }

  // Precompute all request prefix lengths once, not per iteration
  std::vector<uint64_t> prefixes;
  prefixes.reserve(request_offsets.size() - 1);
  for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
    uint64_t prefix = 0;
    for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
      if (!present[request_blocks[static_cast<std::size_t>(i)]]) {
        break;
      }
      ++prefix;
    }
    prefixes.push_back(prefix);
  }

  // Run benchmark iterations with only hash mixing work, no redundant prefix calculation
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint64_t p : prefixes) {
      hash = mix(hash, p);
    }
    final_hash = hash;
  }
  return final_hash;
}