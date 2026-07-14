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
  
  const std::string* dict_data = dict.data();
  const uint32_t* codes_data = codes.data();
  const size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    
    // Unroll by 4 for better ILP
    for (; i + 3 < codes_size; i += 4) {
      const std::string& s0 = dict_data[codes_data[i]];
      const std::string& s1 = dict_data[codes_data[i + 1]];
      const std::string& s2 = dict_data[codes_data[i + 2]];
      const std::string& s3 = dict_data[codes_data[i + 3]];
      
      size_t len0 = s0.size();
      size_t len1 = s1.size();
      size_t len2 = s2.size();
      size_t len3 = s3.size();
      
      unsigned char c0 = len0 ? static_cast<unsigned char>(s0[0]) : 0u;
      unsigned char c1 = len1 ? static_cast<unsigned char>(s1[0]) : 0u;
      unsigned char c2 = len2 ? static_cast<unsigned char>(s2[0]) : 0u;
      unsigned char c3 = len3 ? static_cast<unsigned char>(s3[0]) : 0u;
      
      hash = mix(hash, len0);
      hash = mix(hash, c0);
      hash = mix(hash, len1);
      hash = mix(hash, c1);
      hash = mix(hash, len2);
      hash = mix(hash, c2);
      hash = mix(hash, len3);
      hash = mix(hash, c3);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      const std::string& token = dict_data[codes_data[i]];
      size_t len = token.size();
      hash = mix(hash, len);
      hash = mix(hash, len ? static_cast<unsigned char>(token[0]) : 0u);
    }
  }
  
  return hash;
}