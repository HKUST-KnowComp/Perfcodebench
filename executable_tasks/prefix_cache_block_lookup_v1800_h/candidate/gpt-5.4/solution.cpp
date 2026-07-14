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

  const uint32_t* const cache = sorted_cache_keys.data();
  const std::size_t cache_size = sorted_cache_keys.size();
  const uint32_t* const blocks = request_blocks.data();
  const int* const offsets = request_offsets.data();

  std::vector<uint64_t> prefixes(num_requests);

  if (cache_size == 0) {
    for (std::size_t r = 0; r < num_requests; ++r) {
      prefixes[r] = 0;
    }
  } else if (hash_space != 0 && cache_size >= 32) {
    std::vector<uint8_t> present(static_cast<std::size_t>(hash_space), 0);
    for (std::size_t i = 0; i < cache_size; ++i) {
      present[cache[i]] = 1;
    }

    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        if (!present[blocks[static_cast<std::size_t>(i)]]) {
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
        if (!std::binary_search(cache, cache + cache_size, key)) {
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
