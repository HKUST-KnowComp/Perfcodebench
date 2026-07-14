#include "interface.h"

#include <cstddef>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Fast inline binary search on a sorted uint32_t array.
// Returns true if key is present, false otherwise.
inline bool cache_contains(const uint32_t* arr, std::size_t n, uint32_t key) {
  if (n == 0) return false;
  const uint32_t* base = arr;
  std::size_t lo = 0;
  std::size_t hi = n - 1;
  while (lo < hi) {
    std::size_t mid = lo + (hi - lo) / 2;
    if (base[mid] < key)
      lo = mid + 1;
    else
      hi = mid;
  }
  return base[lo] == key;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  const uint32_t* cache_data = sorted_cache_keys.data();
  const std::size_t cache_size = sorted_cache_keys.size();
  const uint32_t* block_data = request_blocks.data();
  const std::size_t req_count = request_offsets.size() - 1;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < req_count; ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        uint32_t key = block_data[static_cast<std::size_t>(i)];
        if (!cache_contains(cache_data, cache_size, key))
          break;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
