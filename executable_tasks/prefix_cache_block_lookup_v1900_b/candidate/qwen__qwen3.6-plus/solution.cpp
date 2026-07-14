#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct FastCache {
    std::vector<uint32_t> keys;
    std::vector<uint8_t> occ;
    uint32_t mask;

    explicit FastCache(const std::vector<uint32_t>& sorted_keys) {
        uint32_t n = static_cast<uint32_t>(sorted_keys.size());
        if (n == 0) {
            mask = 0;
            return;
        }
        uint32_t size = 1;
        while (size < n * 2) size <<= 1;
        mask = size - 1;
        keys.resize(size, 0);
        occ.resize(size, 0);
        for (uint32_t k : sorted_keys) {
            uint32_t h = k * 0x9e3779b9;
            uint32_t idx = h & mask;
            while (occ[idx]) {
                idx = (idx + 1) & mask;
            }
            keys[idx] = k;
            occ[idx] = 1;
        }
    }

    inline bool contains(uint32_t k) const {
        if (mask == 0) return false;
        uint32_t h = k * 0x9e3779b9;
        uint32_t idx = h & mask;
        while (occ[idx]) {
            if (keys[idx] == k) return true;
            idx = (idx + 1) & mask;
        }
        return false;
    }
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;

  FastCache cache(sorted_cache_keys);
  const uint32_t* blocks = request_blocks.data();
  const int* offsets = request_offsets.data();
  size_t num_requests = request_offsets.size() - 1;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      uint64_t prefix = 0;
      int start = offsets[r];
      int end = offsets[r + 1];
      for (int i = start; i < end; ++i) {
        if (!cache.contains(blocks[i])) break;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}