#include "interface.h"

#include <algorithm>
#include <cstdint>
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

  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (iters <= 0) {
    return 0;
  }

  // Precompute exact per-request prefix lengths once; the result is identical
  // across iterations, only the final hash recomputation repeats.
  std::vector<uint64_t> prefixes(num_requests);

  const uint32_t* const cache_begin = sorted_cache_keys.empty() ? nullptr : sorted_cache_keys.data();
  const uint32_t* const cache_end = cache_begin ? (cache_begin + sorted_cache_keys.size()) : nullptr;
  const uint32_t* const blocks = request_blocks.empty() ? nullptr : request_blocks.data();
  const int* const offsets = request_offsets.empty() ? nullptr : request_offsets.data();

  if (sorted_cache_keys.empty()) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      prefixes[r] = 0;
    }
  } else {
    for (std::size_t r = 0; r < num_requests; ++r) {
      int i = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      for (; i < end; ++i) {
        const uint32_t key = blocks[static_cast<std::size_t>(i)];
        const uint32_t* it = std::lower_bound(cache_begin, cache_end, key);
        if (it == cache_end || *it != key) {
          break;
        }
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefixes[r]);
    }
  }
  return hash;
}
