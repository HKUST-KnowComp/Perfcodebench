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
  
  // Pre-calculate constants to avoid repeated casting in the loop
  const size_t u_max_len = static_cast<size_t>(max_len);
  const size_t u_batch = static_cast<size_t>(batch);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t b = 0; b < u_batch; ++b) {
      const int len = lengths[b];
      const size_t row_offset = b * u_max_len;
      
      for (int i = 0; i < len; ++i) {
        // Instead of pushing to a vector and then iterating, 
        // we mix the values directly into the hash.
        hash = mix(hash, padded_tokens[row_offset + i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}