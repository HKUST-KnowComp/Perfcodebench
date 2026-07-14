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
  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (iters <= 0) return 0;

  // Build a compact membership table when feasible; otherwise fall back to
  // binary search on the sorted cache keys.
  const bool use_table =
      hash_space != 0 &&
      static_cast<uint64_t>(hash_space) <= (1ULL << 27);  // cap memory use

  std::vector<uint8_t> present;
  if (use_table) {
    present.assign(static_cast<std::size_t>(hash_space), static_cast<uint8_t>(0));
    const std::size_t n = sorted_cache_keys.size();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t k = sorted_cache_keys[i];
      if (k < hash_space) present[k] = 1;
    }
  }

  std::vector<uint32_t> prefixes(num_requests);

  if (use_table) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      int begin = request_offsets[r];
      int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        if (key >= hash_space || !present[key]) break;
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  } else {
    const uint32_t* cache = sorted_cache_keys.data();
    const std::size_t cache_size = sorted_cache_keys.size();
    for (std::size_t r = 0; r < num_requests; ++r) {
      int begin = request_offsets[r];
      int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        std::size_t lo = 0;
        std::size_t hi = cache_size;
        while (lo < hi) {
          const std::size_t mid = lo + ((hi - lo) >> 1);
          const uint32_t v = cache[mid];
          if (v < key) {
            lo = mid + 1;
          } else {
            hi = mid;
          }
        }
        if (lo == cache_size || cache[lo] != key) break;
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
