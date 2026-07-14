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
  if (iters <= 0) {
    return 0;
  }

  // Build a compact membership table when the hash space is usable.
  // This turns repeated binary searches into O(1) lookups.
  // If hash_space is too large or inconsistent with the keys, fall back to
  // direct binary search on the sorted cache keys.
  bool use_table = false;
  std::vector<uint8_t> present;
  if (hash_space != 0) {
    bool keys_fit = true;
    for (std::size_t i = 0; i < sorted_cache_keys.size(); ++i) {
      if (sorted_cache_keys[i] >= hash_space) {
        keys_fit = false;
        break;
      }
    }
    if (keys_fit) {
      present.assign(static_cast<std::size_t>(hash_space), uint8_t{0});
      for (std::size_t i = 0; i < sorted_cache_keys.size(); ++i) {
        present[sorted_cache_keys[i]] = uint8_t{1};
      }
      use_table = true;
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
        if (key >= hash_space || !present[key]) {
          break;
        }
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  } else {
    const uint32_t* cache_data = sorted_cache_keys.data();
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
          const uint32_t v = cache_data[mid];
          if (v < key) {
            lo = mid + 1;
          } else {
            hi = mid;
          }
        }
        if (lo == cache_size || cache_data[lo] != key) {
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
