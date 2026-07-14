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
  
  // Allocate once with exact capacity
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_packed);
  positions.reserve(total_packed);
  
  uint64_t hash = 0;
  const uint32_t* padded_ptr = padded_tokens.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    packed_tokens.clear();
    positions.clear();
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[b];
      const uint32_t* row_ptr = padded_ptr + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      
      // Pack and hash in same loop for better cache locality
      int i = 0;
      // Unroll by 4 for better ILP
      for (; i + 3 < len; i += 4) {
        uint32_t t0 = row_ptr[i];
        uint32_t t1 = row_ptr[i + 1];
        uint32_t t2 = row_ptr[i + 2];
        uint32_t t3 = row_ptr[i + 3];
        
        packed_tokens.push_back(t0);
        packed_tokens.push_back(t1);
        packed_tokens.push_back(t2);
        packed_tokens.push_back(t3);
        
        positions.push_back(static_cast<uint32_t>(i));
        positions.push_back(static_cast<uint32_t>(i + 1));
        positions.push_back(static_cast<uint32_t>(i + 2));
        positions.push_back(static_cast<uint32_t>(i + 3));
        
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
        uint32_t token = row_ptr[i];
        packed_tokens.push_back(token);
        positions.push_back(static_cast<uint32_t>(i));
        hash = mix(hash, token);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  
  return hash;
}