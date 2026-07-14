#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

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

  // Build a bitmap for O(1) lookup if hash_space is reasonable
  // Use 64-bit words for the bitmap
  const uint64_t bitmap_bits = static_cast<uint64_t>(hash_space);
  const std::size_t bitmap_words = (bitmap_bits + 63) / 64;
  
  // Threshold: if bitmap would be too large (>64MB), fall back to binary search
  // But typically hash_space should be manageable
  const std::size_t max_bitmap_bytes = 64ULL * 1024 * 1024;
  
  std::vector<uint64_t> bitmap;
  bool use_bitmap = (bitmap_words * sizeof(uint64_t) <= max_bitmap_bytes);
  
  if (use_bitmap) {
    bitmap.resize(bitmap_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitmap[key >> 6] |= (1ULL << (key & 63));
      }
    }
  }

  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const uint64_t* bitmap_ptr = use_bitmap ? bitmap.data() : nullptr;
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();

  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    if (use_bitmap) {
      for (std::size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t key = blocks_ptr[static_cast<std::size_t>(i)];
          // Check bitmap
          uint64_t word_idx = key >> 6;
          uint64_t bit_mask = 1ULL << (key & 63);
          if ((bitmap_ptr[word_idx] & bit_mask) == 0) {
            break;
          }
          ++prefix;
        }
        
        hash = mix(hash, prefix);
      }
    } else {
      // Fallback to binary search for very large hash spaces
      for (std::size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t key = blocks_ptr[static_cast<std::size_t>(i)];
          if (!std::binary_search(cache_begin, cache_end, key)) {
            break;
          }
          ++prefix;
        }
        
        hash = mix(hash, prefix);
      }
    }
  }
  
  return hash;
}