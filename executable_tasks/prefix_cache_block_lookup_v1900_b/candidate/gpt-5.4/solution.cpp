#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t num_requests =
      request_offsets.empty() ? 0 : (request_offsets.size() - 1);

  if (iters <= 0) {
    return 0;
  }

  // Precompute exact prefix lengths once; the baseline recomputes the same
  // result every iteration even though inputs are immutable.
  std::vector<uint64_t> prefixes(num_requests);

  const uint32_t* const cache_begin = sorted_cache_keys.data();
  const uint32_t* const cache_end = cache_begin + sorted_cache_keys.size();
  const uint32_t* const blocks = request_blocks.data();
  const int* const offsets = request_offsets.data();

  // Fast path when the cache is dense over [0, hash_space). In that case
  // membership is equivalent to key < hash_space, avoiding binary search.
  bool dense_full_space = false;
  if (hash_space != 0 && sorted_cache_keys.size() == static_cast<std::size_t>(hash_space)) {
    dense_full_space = true;
    for (std::size_t i = 0; i < sorted_cache_keys.size(); ++i) {
      if (sorted_cache_keys[i] != static_cast<uint32_t>(i)) {
        dense_full_space = false;
        break;
      }
    }
  }

  if (dense_full_space) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        if (blocks[static_cast<std::size_t>(i)] >= hash_space) {
          break;
        }
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  } else {
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = begin; i < end; ++i) {
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
