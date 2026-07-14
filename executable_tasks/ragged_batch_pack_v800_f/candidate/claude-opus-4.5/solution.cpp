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
      const uint32_t* __restrict__ row = tokens_ptr + static_cast<std::size_t>(b) * max_len_sz;
      
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, row[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  
  return hash;
}