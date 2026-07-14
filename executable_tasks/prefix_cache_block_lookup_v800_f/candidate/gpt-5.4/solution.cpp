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
  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (iters <= 0) {
    return 0;
  }

  // Build a compact membership structure once and reuse across iterations.
  // Prefer a dense bitset when the hash space is reasonable; otherwise fall back
  // to a sorted unique copy and binary search.
  const bool use_dense =
      hash_space != 0 &&
      static_cast<uint64_t>(hash_space) <= (1ULL << 27);  // up to 16 MiB bitset

  std::vector<uint64_t> dense_bits;
  std::vector<uint32_t> keys;

  if (use_dense) {
    dense_bits.assign((static_cast<std::size_t>(hash_space) + 63u) >> 6, 0ULL);
    for (uint32_t k : sorted_cache_keys) {
      if (k < hash_space) {
        dense_bits[static_cast<std::size_t>(k) >> 6] |=
            1ULL << (static_cast<unsigned>(k) & 63u);
      }
    }
  } else {
    keys = sorted_cache_keys;
    if (!keys.empty()) {
      auto it = std::unique(keys.begin(), keys.end());
      keys.erase(it, keys.end());
    }
  }

  std::vector<uint32_t> prefixes(num_requests);

  if (use_dense) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      int begin = request_offsets[r];
      int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t k = request_blocks[static_cast<std::size_t>(i)];
        if (k >= hash_space) break;
        const uint64_t word = dense_bits[static_cast<std::size_t>(k) >> 6];
        if (((word >> (static_cast<unsigned>(k) & 63u)) & 1ULL) == 0ULL) break;
        ++prefix;
      }
      prefixes[r] = prefix;
    }
  } else {
    const uint32_t* key_data = keys.empty() ? nullptr : keys.data();
    const std::size_t key_size = keys.size();
    for (std::size_t r = 0; r < num_requests; ++r) {
      int begin = request_offsets[r];
      int end = request_offsets[r + 1];
      uint32_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        const uint32_t k = request_blocks[static_cast<std::size_t>(i)];
        const bool found = std::binary_search(key_data, key_data + key_size, k);
        if (!found) break;
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
