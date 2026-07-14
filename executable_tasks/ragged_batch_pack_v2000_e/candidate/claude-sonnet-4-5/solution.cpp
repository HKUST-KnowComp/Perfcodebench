#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total size
  size_t total_size = 0;
  for (int b = 0; b < batch; ++b) {
    total_size += lengths[b];
  }
  
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_size);
  positions.reserve(total_size);
  
  uint64_t hash = 0;
  const uint32_t* padded_ptr = padded_tokens.data();
  const int* lengths_ptr = lengths.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    
    for (int b = 0; b < batch; ++b) {
      int len = lengths_ptr[b];
      const uint32_t* row_ptr = padded_ptr + b * max_len;
      
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(row_ptr[i]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    
    // Unrolled hash computation with independent chains
    hash = 1469598103934665603ULL;
    uint64_t hash1 = hash, hash2 = hash, hash3 = hash, hash4 = hash;
    
    const uint32_t* tok_ptr = packed_tokens.data();
    const uint32_t* pos_ptr = positions.data();
    size_t size = packed_tokens.size();
    size_t i = 0;
    
    // Process 4 elements at a time
    for (; i + 3 < size; i += 4) {
      hash1 = mix(hash1, tok_ptr[i]);
      hash1 = mix(hash1, pos_ptr[i]);
      
      hash2 = mix(hash2, tok_ptr[i+1]);
      hash2 = mix(hash2, pos_ptr[i+1]);
      
      hash3 = mix(hash3, tok_ptr[i+2]);
      hash3 = mix(hash3, pos_ptr[i+2]);
      
      hash4 = mix(hash4, tok_ptr[i+3]);
      hash4 = mix(hash4, pos_ptr[i+3]);
    }
    
    // Combine independent chains
    hash = mix(mix(hash1, hash2), mix(hash3, hash4));
    
    // Handle remaining elements
    for (; i < size; ++i) {
      hash = mix(hash, tok_ptr[i]);
      hash = mix(hash, pos_ptr[i]);
    }
  }
  return hash;
}