#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

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
  const std::size_t codes_size = codes.size();
  const uint32_t* codes_data = codes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = codes_size & ~3ULL;
    
    for (; i < unroll_limit; i += 4) {
      const std::string& token0 = dict[codes_data[i]];
      const std::string& token1 = dict[codes_data[i + 1]];
      const std::string& token2 = dict[codes_data[i + 2]];
      const std::string& token3 = dict[codes_data[i + 3]];
      
      std::size_t len0 = token0.size();
      std::size_t len1 = token1.size();
      std::size_t len2 = token2.size();
      std::size_t len3 = token3.size();
      
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
    
    for (; i < codes_size; ++i) {
      const std::string& token = dict[codes_data[i]];
      std::size_t len = token.size();
      hash = mix(hash, len);
      hash = mix(hash, len ? static_cast<unsigned char>(token[0]) : 0u);
    }
  }
  
  return hash;
}