#include "interface.h"

#include <cstddef>

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  const uint32_t* cache_data = sorted_cache_keys.data();
  size_t cache_size = sorted_cache_keys.size();
  const uint32_t* blocks = request_blocks.data();
  const int* offsets = request_offsets.data();
  size_t num_requests = request_offsets.size() - 1;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      uint64_t prefix = 0;
      size_t start = static_cast<size_t>(offsets[r]);
      size_t end = static_cast<size_t>(offsets[r+1]);
      for (size_t i = start; i < end; ++i) {
        uint32_t key = blocks[i];
        size_t lo = 0;
        size_t hi = cache_size;
        bool found = false;
        while (lo < hi) {
          size_t mid = (lo + hi) >> 1;
          uint32_t val = cache_data[mid];
          if (val == key) {
            found = true;
            break;
          }
          if (val < key) lo = mid + 1;
          else hi = mid;
        }
        if (!found) break;
        ++prefix;
      }
      hash ^= prefix;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}