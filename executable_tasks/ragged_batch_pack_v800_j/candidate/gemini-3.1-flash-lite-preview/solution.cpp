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
  uint64_t final_hash = 0;
  
  // Pre-calculate total size to avoid reallocations
  size_t total_elements = 0;
  for (int len : lengths) {
    total_elements += static_cast<size_t>(len);
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    const uint32_t* src_ptr = padded_tokens.data();
    for (int b = 0; b < batch; ++b) {
      int len = lengths[b];
      for (int i = 0; i < len; ++i) {
        // Pack and hash in a single pass to maximize cache locality
        uint32_t token = src_ptr[i];
        hash = mix(hash, token);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
      src_ptr += max_len;
    }
    final_hash = hash;
  }
  return final_hash;
}