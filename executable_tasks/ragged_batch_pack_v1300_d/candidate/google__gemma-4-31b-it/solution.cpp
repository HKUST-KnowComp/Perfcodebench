#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  uint64_t final_hash = 0;
  
  // Pre-calculate total tokens to avoid repeated length access if possible, 
  // though the inner loop is the bottleneck.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[b];
      const size_t row_offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      
      for (int i = 0; i < len; ++i) {
        // The original code pushed token then position
        hash = mix(hash, padded_tokens[row_offset + i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}