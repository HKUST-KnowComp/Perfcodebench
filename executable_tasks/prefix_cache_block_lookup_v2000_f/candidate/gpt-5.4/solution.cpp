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
  (void)hash_space;

  const std::size_t num_requests =
      request_offsets.size() > 0 ? (request_offsets.size() - 1) : 0;
  if (iters <= 0) {
    return 0;
  }

  const std::size_t cache_size = sorted_cache_keys.size();
  const uint32_t* const cache = cache_size ? sorted_cache_keys.data() : nullptr;
  const uint32_t* const blocks = request_blocks.empty() ? nullptr : request_blocks.data();
  const int* const offsets = request_offsets.empty() ? nullptr : request_offsets.data();

  std::vector<uint64_t> prefix_lengths(num_requests);

  for (std::size_t r = 0; r < num_requests; ++r) {
    const int begin_i = offsets[r];
    const int end_i = offsets[r + 1];
    uint64_t prefix = 0;

    for (int i = begin_i; i < end_i; ++i) {
      const uint32_t key = blocks[static_cast<std::size_t>(i)];

      const uint32_t* it = std::lower_bound(cache, cache + cache_size, key);
      if (it == cache + cache_size || *it != key) {
        break;
      }
      ++prefix;
    }

    prefix_lengths[r] = prefix;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefix_lengths[r]);
    }
  }
  return hash;
}
