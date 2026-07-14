#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed length
  const int* len_ptr = lengths.data();
  std::size_t total_len = 0;
  for (int b = 0; b < batch; ++b) {
    total_len += static_cast<std::size_t>(len_ptr[b]);
  }
  
  // Pre-allocate vectors once
  std::vector<uint32_t> packed_tokens(total_len);
  std::vector<uint32_t> positions(total_len);
  
  const uint32_t* src = padded_tokens.data();
  const std::size_t ml = static_cast<std::size_t>(max_len);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* pack_ptr = packed_tokens.data();
    uint32_t* pos_ptr = positions.data();
    
    // Pack tokens and positions
    for (int b = 0; b < batch; ++b) {
      const int len = len_ptr[b];
      const uint32_t* row = src + static_cast<std::size_t>(b) * ml;
      
      for (int i = 0; i < len; ++i) {
        *pack_ptr++ = row[i];
        *pos_ptr++ = static_cast<uint32_t>(i);
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* pt = packed_tokens.data();
    const uint32_t* pp = positions.data();
    
    std::size_t i = 0;
    const std::size_t unroll_end = total_len & ~static_cast<std::size_t>(3);
    
    // Unrolled loop
    for (; i < unroll_end; i += 4) {
      hash = mix(hash, pt[i]);
      hash = mix(hash, pp[i]);
      hash = mix(hash, pt[i + 1]);
      hash = mix(hash, pp[i + 1]);
      hash = mix(hash, pt[i + 2]);
      hash = mix(hash, pp[i + 2]);
      hash = mix(hash, pt[i + 3]);
      hash = mix(hash, pp[i + 3]);
    }
    
    // Remainder
    for (; i < total_len; ++i) {
      hash = mix(hash, pt[i]);
      hash = mix(hash, pp[i]);
    }
  }
  
  return hash;
}