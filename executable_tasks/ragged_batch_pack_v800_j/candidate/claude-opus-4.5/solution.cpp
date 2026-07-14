#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Compute total packed size
  size_t total_packed = 0;
  for (int b = 0; b < batch; ++b) {
    total_packed += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  // Pre-allocate once
  std::vector<uint32_t> packed_tokens(total_packed);
  std::vector<uint32_t> positions(total_packed);
  
  // Compute prefix sums for batch offsets
  std::vector<size_t> offsets(static_cast<size_t>(batch) + 1);
  offsets[0] = 0;
  for (int b = 0; b < batch; ++b) {
    offsets[static_cast<size_t>(b) + 1] = offsets[static_cast<size_t>(b)] + static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  uint64_t hash = 0;
  const uint32_t* src_base = padded_tokens.data();
  const size_t ml = static_cast<size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens and positions
    uint32_t* packed_ptr = packed_tokens.data();
    uint32_t* pos_ptr = positions.data();
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      const size_t out_off = offsets[static_cast<size_t>(b)];
      const uint32_t* src = src_base + static_cast<size_t>(b) * ml;
      
      // Copy tokens using memcpy for better performance
      std::memcpy(packed_ptr + out_off, src, static_cast<size_t>(len) * sizeof(uint32_t));
      
      // Fill positions 0, 1, 2, ..., len-1
      uint32_t* pos_out = pos_ptr + out_off;
      for (int i = 0; i < len; ++i) {
        pos_out[i] = static_cast<uint32_t>(i);
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* tok = packed_tokens.data();
    const uint32_t* pos = positions.data();
    
    for (size_t i = 0; i < total_packed; ++i) {
      hash = mix(hash, tok[i]);
      hash = mix(hash, pos[i]);
    }
  }
  
  return hash;
}