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
  std::size_t total_size = 0;
  for (int b = 0; b < batch; ++b) {
    total_size += static_cast<std::size_t>(lengths[b]);
  }
  
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_size);
  positions.reserve(total_size);
  
  uint64_t hash = 0;
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[b];
      const uint32_t* row_base = &padded_tokens[static_cast<std::size_t>(b) * max_len_sz];
      
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(row_base[i]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    
    hash = 1469598103934665603ULL;
    const std::size_t size = packed_tokens.size();
    std::size_t i = 0;
    
    // Unroll by 4
    for (; i + 3 < size; i += 4) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
      hash = mix(hash, packed_tokens[i + 1]);
      hash = mix(hash, positions[i + 1]);
      hash = mix(hash, packed_tokens[i + 2]);
      hash = mix(hash, positions[i + 2]);
      hash = mix(hash, packed_tokens[i + 3]);
      hash = mix(hash, positions[i + 3]);
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  
  return hash;
}