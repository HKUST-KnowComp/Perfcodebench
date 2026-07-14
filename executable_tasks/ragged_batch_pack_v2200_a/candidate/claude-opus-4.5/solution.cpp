#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed size
  std::size_t total_packed = 0;
  for (int b = 0; b < batch; ++b) {
    total_packed += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
  }
  
  // Pre-allocate vectors once
  std::vector<uint32_t> packed_tokens(total_packed);
  std::vector<uint32_t> positions(total_packed);
  
  const uint32_t* __restrict__ src = padded_tokens.data();
  const int* __restrict__ lens = lengths.data();
  
  uint64_t hash = 0;
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens and positions
    uint32_t* __restrict__ tok_out = packed_tokens.data();
    uint32_t* __restrict__ pos_out = positions.data();
    std::size_t out_idx = 0;
    
    for (int b = 0; b < batch; ++b) {
      const int len = lens[b];
      const uint32_t* __restrict__ row = src + static_cast<std::size_t>(b) * max_len_sz;
      for (int i = 0; i < len; ++i) {
        tok_out[out_idx] = row[i];
        pos_out[out_idx] = static_cast<uint32_t>(i);
        ++out_idx;
      }
    }
    
    // Compute hash - FNV-1a variant
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_packed; ++i) {
      hash ^= tok_out[i];
      hash *= 1099511628211ULL;
      hash ^= pos_out[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}