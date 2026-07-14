#include "interface.h"
#include <vector>
#include <cstdint>

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
  
  const size_t n = sorted_cache_keys.size();
  const size_t num_requests = request_offsets.size();
  const uint32_t* blocks = request_blocks.data();
  const int* offsets = request_offsets.data();
  
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      for (size_t r = 0; r + 1 < num_requests; ++r) {
        hash = mix(hash, 0);
      }
    }
    return hash;
  }
  
  size_t cap = 1;
  while (cap < n * 2) cap <<= 1;
  
  std::vector<uint32_t> ht_keys(cap);
  std::vector<uint8_t> ht_used(cap, 0);
  uint32_t mask = static_cast<uint32_t>(cap - 1);
  
  const uint32_t* src = sorted_cache_keys.data();
  uint32_t* k_ptr = ht_keys.data();
  uint8_t* u_ptr = ht_used.data();
  
  for (size_t i = 0; i < n; ++i) {
    uint32_t k = src[i];
    uint32_t h = k * 0x9E3779B9;
    uint32_t idx = h & mask;
    while (u_ptr[idx]) {
      idx = (idx + 1) & mask;
    }
    k_ptr[idx] = k;
    u_ptr[idx] = 1;
  }
  
  const uint32_t* k_data = ht_keys.data();
  const uint8_t* u_data = ht_used.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < num_requests; ++r) {
      uint64_t prefix = 0;
      int start = offsets[r];
      int end = offsets[r + 1];
      for (int i = start; i < end; ++i) {
        uint32_t k = blocks[static_cast<size_t>(i)];
        uint32_t h = k * 0x9E3779B9;
        uint32_t idx = h & mask;
        bool found = false;
        while (u_data[idx]) {
          if (k_data[idx] == k) {
            found = true;
            break;
          }
          idx = (idx + 1) & mask;
        }
        if (!found) break;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}