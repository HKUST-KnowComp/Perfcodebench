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
  const uint32_t* __restrict__ tokens_ptr = padded_tokens.data();
  const int* __restrict__ lengths_ptr = lengths.data();
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths_ptr[b];
      const uint32_t* row = tokens_ptr + static_cast<std::size_t>(b) * max_len_sz;
      
      // Prefetch next row
      if (b + 1 < batch) {
        __builtin_prefetch(tokens_ptr + static_cast<std::size_t>(b + 1) * max_len_sz, 0, 3);
      }
      
      // Process in chunks of 4 for better ILP
      int i = 0;
      const int len4 = len & ~3;
      
      for (; i < len4; i += 4) {
        uint32_t t0 = row[i];
        uint32_t t1 = row[i + 1];
        uint32_t t2 = row[i + 2];
        uint32_t t3 = row[i + 3];
        
        hash = mix(hash, t0);
        hash = mix(hash, static_cast<uint32_t>(i));
        hash = mix(hash, t1);
        hash = mix(hash, static_cast<uint32_t>(i + 1));
        hash = mix(hash, t2);
        hash = mix(hash, static_cast<uint32_t>(i + 2));
        hash = mix(hash, t3);
        hash = mix(hash, static_cast<uint32_t>(i + 3));
      }
      
      // Handle remainder
      for (; i < len; ++i) {
        hash = mix(hash, row[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  
  return hash;
}