#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      auto begin = request_blocks.begin() + request_offsets[r];
      auto end = request_blocks.begin() + request_offsets[r + 1];
      while (begin != end) {
        auto it = std::lower_bound(begin, end, *begin);
        if (it == end || *it != *begin) {
          break;
        }
        prefix += it - begin;
        begin = it + 1;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
