#include "interface.h"

#include <vector>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed length
  std::size_t total_len = 0;
  for (int b = 0; b < batch; ++b) {
    total_len += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
  }
  
  // Pre-allocate vectors once
  std::vector<uint32_t> packed_tokens(total_len);
  std::vector<uint32_t> positions(total_len);
  
  // Pre-compute row offsets for padded_tokens
  const uint32_t* padded_base = padded_tokens.data();
  const int* lengths_ptr = lengths.data();
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* packed_ptr = packed_tokens.data();
    uint32_t* pos_ptr = positions.data();
    
    // Pack all batches
    for (int b = 0; b < batch; ++b) {
      const int len = lengths_ptr[b];
      const uint32_t* row = padded_base + static_cast<std::size_t>(b) * max_len_sz;
      
      for (int i = 0; i < len; ++i) {
        *packed_ptr++ = row[i];
        *pos_ptr++ = static_cast<uint32_t>(i);
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const uint32_t* tok_ptr = packed_tokens.data();
    const uint32_t* p_ptr = positions.data();
    
    for (std::size_t i = 0; i < total_len; ++i) {
      hash = mix(hash, tok_ptr[i]);
      hash = mix(hash, p_ptr[i]);
    }
  }
  
  return hash;
}