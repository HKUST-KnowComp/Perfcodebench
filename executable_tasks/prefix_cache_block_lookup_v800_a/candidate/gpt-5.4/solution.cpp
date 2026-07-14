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

  // Build a compact membership table when the hash space is reasonable.
  // This turns per-block lookup from binary_search into a single indexed load.
  // Fall back to binary_search when the table would be too large or unusable.
  constexpr uint32_t kMaxDirectTable = 1u << 26;  // 64M bytes as uint8_t table.
  const bool use_direct_table =
      hash_space != 0 && hash_space <= kMaxDirectTable;

  std::vector<uint8_t> present;
  if (use_direct_table) {
    present.assign(static_cast<std::size_t>(hash_space), uint8_t{0});
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        present[static_cast<std::size_t>(key)] = 1;
      }
    }
  }

  uint64_t hash = 0;

  if (use_direct_table) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      for (std::size_t r = 0; r < num_requests; ++r) {
        const int begin_i = request_offsets[r];
        const int end_i = request_offsets[r + 1];
        uint64_t prefix = 0;

        int i = begin_i;
        for (; i < end_i; ++i) {
          const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
          if (key >= hash_space || !present[static_cast<std::size_t>(key)]) {
            break;
          }
          ++prefix;
        }

        hash = mix(hash, prefix);
      }
    }
    return hash;
  }

  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin_i = request_offsets[r];
      const int end_i = request_offsets[r + 1];
      uint64_t prefix = 0;

      for (int i = begin_i; i < end_i; ++i) {
        const uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        if (!std::binary_search(cache_begin, cache_end, key)) {
          break;
        }
        ++prefix;
      }

      hash = mix(hash, prefix);
    }
  }

  return hash;
}
