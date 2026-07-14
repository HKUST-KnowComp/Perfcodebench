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
  int total_packed = 0;
  for (int b = 0; b < batch; ++b) {
    total_packed += lengths[b];
  }
  
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_packed);
  positions.reserve(total_packed);
  
  uint64_t hash = 0;
  const uint32_t* padded_ptr = padded_tokens.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    
    for (int b = 0; b < batch; ++b) {
      int len = lengths[b];
      const uint32_t* row_ptr = padded_ptr + b * max_len;
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(row_ptr[i]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    
    hash = 1469598103934665603ULL;
    std::size_t size = packed_tokens.size();
    std::size_t i = 0;
    
    // Unroll hash loop 4-way
    for (; i + 3 < size; i += 4) {
      uint64_t h0 = hash;
      uint64_t h1 = hash;
      uint64_t h2 = hash;
      uint64_t h3 = hash;
      
      h0 = mix(h0, packed_tokens[i]);
      h0 = mix(h0, positions[i]);
      
      h1 = mix(h1, packed_tokens[i+1]);
      h1 = mix(h1, positions[i+1]);
      
      h2 = mix(h2, packed_tokens[i+2]);
      h2 = mix(h2, positions[i+2]);
      
      h3 = mix(h3, packed_tokens[i+3]);
      h3 = mix(h3, positions[i+3]);
      
      hash = h0 ^ h1 ^ h2 ^ h3;
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}