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
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    
    for (int b = 0; b < batch; ++b) {
      const std::size_t base_offset = static_cast<std::size_t>(b) * max_len_sz;
      const int len = lengths[b];
      
      for (int i = 0; i < len; ++i) {
        packed_tokens.push_back(padded_tokens[base_offset + static_cast<std::size_t>(i)]);
        positions.push_back(static_cast<uint32_t>(i));
      }
    }
    
    hash = 1469598103934665603ULL;
    const std::size_t packed_size = packed_tokens.size();
    for (std::size_t i = 0; i < packed_size; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  
  return hash;
}