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
  const std::size_t num_keys = sorted_cache_keys.size();
  const std::size_t num_blocks = request_blocks.size();
  const std::size_t num_offsets = request_offsets.size();

  if (iters <= 0) {
    return 0;
  }

  // Build a compact membership structure once and reuse across iterations.
  // For dense/small hash spaces, a direct bitmap is fastest.
  // Otherwise, use a sorted unique copy and perform galloping-aware lower_bound
  // over the monotone request prefix scans.
  const bool use_bitmap =
      hash_space != 0 && static_cast<uint64_t>(hash_space) <= (static_cast<uint64_t>(num_keys) << 6);

  std::vector<uint8_t> bitmap;
  std::vector<uint32_t> keys;

  if (use_bitmap) {
    bitmap.assign(static_cast<std::size_t>(hash_space), static_cast<uint8_t>(0));
    for (std::size_t i = 0; i < num_keys; ++i) {
      const uint32_t k = sorted_cache_keys[i];
      if (k < hash_space) bitmap[k] = 1;
    }
  } else {
    keys = sorted_cache_keys;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (std::size_t r = 0; r + 1 < num_offsets; ++r) {
      int begin_i = request_offsets[r];
      int end_i = request_offsets[r + 1];
      if (begin_i >= end_i) {
        hash = mix(hash, 0);
        continue;
      }

      uint64_t prefix = 0;

      if (use_bitmap) {
        for (int i = begin_i; i < end_i; ++i) {
          const uint32_t v = request_blocks[static_cast<std::size_t>(i)];
          if (v >= hash_space || bitmap[v] == 0) break;
          ++prefix;
        }
      } else {
        const uint32_t* key_data = keys.data();
        std::size_t lo = 0;
        std::size_t hi = keys.size();

        for (int i = begin_i; i < end_i; ++i) {
          const uint32_t v = request_blocks[static_cast<std::size_t>(i)];

          std::size_t pos = lo;
          if (pos < hi && key_data[pos] < v) {
            std::size_t step = 1;
            std::size_t new_pos = pos + step;
            while (new_pos < hi && key_data[new_pos] < v) {
              pos = new_pos;
              step <<= 1;
              new_pos = pos + step;
            }
            std::size_t left = pos + 1;
            std::size_t right = (new_pos < hi) ? new_pos : hi;
            pos = static_cast<std::size_t>(std::lower_bound(key_data + left, key_data + right, v) - key_data);
          } else {
            pos = static_cast<std::size_t>(std::lower_bound(key_data + lo, key_data + hi, v) - key_data);
          }

          if (pos >= hi || key_data[pos] != v) break;
          lo = pos;
          ++prefix;
        }
      }

      hash = mix(hash, prefix);
    }
  }

  (void)num_blocks;
  return hash;
}
