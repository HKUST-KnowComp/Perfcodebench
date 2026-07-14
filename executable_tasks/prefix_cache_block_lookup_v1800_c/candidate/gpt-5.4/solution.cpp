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
      request_offsets.size() > 0 ? (request_offsets.size() - 1) : 0;

  if (iters <= 0) {
    return 0;
  }

  // Build a compact membership table when the hash/key space is small enough.
  // This turns repeated binary searches into O(1) lookups, which is especially
  // beneficial because the same requests are replayed for every iteration.
  const bool can_use_table =
      hash_space != 0 &&
      static_cast<uint64_t>(hash_space) <= (1ULL << 27);  // <= 128 MiB bytes

  std::vector<unsigned char> present;
  if (can_use_table) {
    present.assign(static_cast<std::size_t>(hash_space), 0);
    for (uint32_t k : sorted_cache_keys) {
      if (k < hash_space) {
        present[static_cast<std::size_t>(k)] = 1;
      }
    }
  }

  // Precompute exact per-request prefix lengths once; iterations only replay the
  // checksum mixing with identical values.
  std::vector<uint32_t> prefixes(num_requests);

  if (can_use_table) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = request_offsets[r];
      const int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        if (key >= hash_space || !present[static_cast<std::size_t>(key)]) {
          break;
        }
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  } else {
    const uint32_t* cache_begin = sorted_cache_keys.data();
    const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = request_offsets[r];
      const int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        if (!std::binary_search(cache_begin, cache_end, key)) {
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
