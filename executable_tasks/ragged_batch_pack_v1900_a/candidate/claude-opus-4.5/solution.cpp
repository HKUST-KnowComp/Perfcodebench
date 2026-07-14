#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  
  const uint32_t* __restrict__ tokens_ptr = padded_tokens.data();
  const int* __restrict__ lengths_ptr = lengths.data();
  const std::size_t max_len_sz = static_cast<std::size_t>(max_len);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths_ptr[b];
      const uint32_t* row = tokens_ptr + static_cast<std::size_t>(b) * max_len_sz;
      
      int i = 0;
      // Unroll by 4
      for (; i + 3 < len; i += 4) {
        hash ^= row[i];
        hash *= FNV_PRIME;
        hash ^= static_cast<uint32_t>(i);
        hash *= FNV_PRIME;
        
        hash ^= row[i + 1];
        hash *= FNV_PRIME;
        hash ^= static_cast<uint32_t>(i + 1);
        hash *= FNV_PRIME;
        
        hash ^= row[i + 2];
        hash *= FNV_PRIME;
        hash ^= static_cast<uint32_t>(i + 2);
        hash *= FNV_PRIME;
        
        hash ^= row[i + 3];
        hash *= FNV_PRIME;
        hash ^= static_cast<uint32_t>(i + 3);
        hash *= FNV_PRIME;
      }
      
      // Handle remainder
      for (; i < len; ++i) {
        hash ^= row[i];
        hash *= FNV_PRIME;
        hash ^= static_cast<uint32_t>(i);
        hash *= FNV_PRIME;
      }
    }
  }
  
  return hash;
}