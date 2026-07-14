#include "interface.h"

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

  // Build a compact membership structure once and reuse across iterations.
  // For small-request throughput, avoiding repeated binary_search calls is key.
  uint64_t hash = 0;

  if (hash_space != 0) {
    // Dense direct-address table when key space is bounded by hash_space.
    std::vector<uint8_t> present(static_cast<std::size_t>(hash_space), 0);
    for (uint32_t k : sorted_cache_keys) {
      if (k < hash_space) {
        present[static_cast<std::size_t>(k)] = 1;
      }
    }

    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      for (std::size_t r = 0; r < num_requests; ++r) {
        const int begin = request_offsets[r];
        const int end = request_offsets[r + 1];
        uint64_t prefix = 0;
        for (int i = begin; i < end; ++i) {
          const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
          if (key >= hash_space ||
              !present[static_cast<std::size_t>(key)]) {
            break;
          }
          ++prefix;
        }
        hash = mix(hash, prefix);
      }
    }
    return hash;
  }

  // Fallback: if hash_space is unusable, use a sorted linear scan with a moving
  // lower_bound start to reduce repeated work versus full-range binary_search.
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = request_offsets[r];
      const int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      std::size_t lo = 0;
      std::size_t hi = sorted_cache_keys.size();
      for (int i = begin; i < end; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        std::size_t l = lo;
        std::size_t h = hi;
        while (l < h) {
          const std::size_t m = l + ((h - l) >> 1);
          if (sorted_cache_keys[m] < key) {
            l = m + 1;
          } else {
            h = m;
          }
        }
        if (l == sorted_cache_keys.size() || sorted_cache_keys[l] != key) {
          break;
        }
        lo = l;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }

  return hash;
}
