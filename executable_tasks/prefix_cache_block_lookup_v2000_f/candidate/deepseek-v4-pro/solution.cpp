#include "interface.h"
#include <unordered_set>
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

  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  cache_set.insert(sorted_cache_keys.begin(), sorted_cache_keys.end());

  std::vector<uint64_t> prefix_lengths;
  prefix_lengths.reserve(request_offsets.size() - 1);

  for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
    uint64_t prefix = 0;
    for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
      if (!cache_set.count(request_blocks[static_cast<std::size_t>(i)])) {
        break;
      }
      ++prefix;
    }
    prefix_lengths.push_back(prefix);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t len : prefix_lengths) {
      hash = mix(hash, len);
    }
  }

  return hash;
}
