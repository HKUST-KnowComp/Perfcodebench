#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (num_requests == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }
  
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  const int* offsets_data = request_offsets.data();
  
  // Use bitset for O(1) lookup if hash_space is reasonable
  constexpr uint32_t MAX_BITSET_SIZE = 64 * 1024 * 1024; // 64M entries = 8MB memory
  std::vector<uint64_t> bitset;
  bool use_bitset = false;
  
  if (hash_space > 0 && hash_space <= MAX_BITSET_SIZE && !sorted_cache_keys.empty()) {
    use_bitset = true;
    size_t num_words = (hash_space + 63) / 64;
    bitset.resize(num_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }
  }
  
  // Precompute prefix lengths once
  std::vector<uint64_t> prefix_lengths(num_requests);
  
  if (use_bitset) {
    const uint64_t* bitset_data = bitset.data();
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_data[r];
      int end = offsets_data[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_data[i];
        uint64_t word = bitset_data[block >> 6];
        if (__builtin_expect((word & (1ULL << (block & 63))) == 0, 0)) {
          break;
        }
        ++prefix;
      }
      prefix_lengths[r] = prefix;
    }
  } else {
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_data[r];
      int end = offsets_data[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_data[i];
        // Binary search
        const uint32_t* it = std::lower_bound(cache_begin, cache_end, block);
        if (__builtin_expect(it == cache_end || *it != block, 0)) {
          break;
        }
        ++prefix;
      }
      prefix_lengths[r] = prefix;
    }
  }
  
  // Compute hash for all iterations
  uint64_t hash = 0;
  const uint64_t* prefix_data = prefix_lengths.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefix_data[r]);
    }
  }
  
  return hash;
}