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
  // Pre-compute total packed size
  std::size_t total_packed = 0;
  for (int b = 0; b < batch; ++b) {
    total_packed += static_cast<std::size_t>(lengths[b]);
  }
  
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_packed);
  positions.reserve(total_packed);
  
  uint64_t hash = 0;
  const uint32_t* padded_ptr = padded_tokens.data();
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[b];
      const uint32_t* row_ptr = padded_ptr + static_cast<std::size_t>(b) * max_len_sz;
      
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(row_ptr[i]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    
    hash = 1469598103934665603ULL;
    const std::size_t size = packed_tokens.size();
    const uint32_t* tok_ptr = packed_tokens.data();
    const uint32_t* pos_ptr = positions.data();
    
    std::size_t i = 0;
    // Unroll by 4 with independent hash chains
    uint64_t h0 = hash, h1 = hash, h2 = hash, h3 = hash;
    for (; i + 3 < size; i += 4) {
      h0 = mix(h0, tok_ptr[i]);
      h0 = mix(h0, pos_ptr[i]);
      
      h1 = mix(h1, tok_ptr[i+1]);
      h1 = mix(h1, pos_ptr[i+1]);
      
      h2 = mix(h2, tok_ptr[i+2]);
      h2 = mix(h2, pos_ptr[i+2]);
      
      h3 = mix(h3, tok_ptr[i+3]);
      h3 = mix(h3, pos_ptr[i+3]);
    }
    
    // Combine hash chains
    hash = h0;
    hash = mix(hash, h1);
    hash = mix(hash, h2);
    hash = mix(hash, h3);
    
    // Handle remainder
    for (; i < size; ++i) {
      hash = mix(hash, tok_ptr[i]);
      hash = mix(hash, pos_ptr[i]);
    }
  }
  return hash;
}