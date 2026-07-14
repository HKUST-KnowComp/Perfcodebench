#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  uint64_t hash = 0;
  const size_t codes_size = codes.size();
  const uint32_t* codes_ptr = codes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    
    // Unroll by 4 for better ILP
    for (; i + 3 < codes_size; i += 4) {
      uint32_t code0 = codes_ptr[i];
      uint32_t code1 = codes_ptr[i + 1];
      uint32_t code2 = codes_ptr[i + 2];
      uint32_t code3 = codes_ptr[i + 3];
      
      const std::string& token0 = dict[code0];
      const std::string& token1 = dict[code1];
      const std::string& token2 = dict[code2];
      const std::string& token3 = dict[code3];
      
      size_t len0 = token0.size();
      size_t len1 = token1.size();
      size_t len2 = token2.size();
      size_t len3 = token3.size();
      
      unsigned char first0 = len0 ? static_cast<unsigned char>(token0[0]) : 0u;
      unsigned char first1 = len1 ? static_cast<unsigned char>(token1[0]) : 0u;
      unsigned char first2 = len2 ? static_cast<unsigned char>(token2[0]) : 0u;
      unsigned char first3 = len3 ? static_cast<unsigned char>(token3[0]) : 0u;
      
      hash = mix(hash, len0);
      hash = mix(hash, first0);
      hash = mix(hash, len1);
      hash = mix(hash, first1);
      hash = mix(hash, len2);
      hash = mix(hash, first2);
      hash = mix(hash, len3);
      hash = mix(hash, first3);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      uint32_t code = codes_ptr[i];
      const std::string& token = dict[code];
      size_t len = token.size();
      unsigned char first = len ? static_cast<unsigned char>(token[0]) : 0u;
      hash = mix(hash, len);
      hash = mix(hash, first);
    }
  }
  
  return hash;
}
