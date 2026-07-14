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
    
    // Process 4 codes at a time for better ILP
    for (; i + 3 < codes_size; i += 4) {
      uint32_t code0 = codes_data[i];
      uint32_t code1 = codes_data[i + 1];
      uint32_t code2 = codes_data[i + 2];
      uint32_t code3 = codes_data[i + 3];
      
      const std::string& s0 = dict_data[code0];
      const std::string& s1 = dict_data[code1];
      const std::string& s2 = dict_data[code2];
      const std::string& s3 = dict_data[code3];
      
      // Prefetch next batch
      if (i + 7 < codes_size) {
        __builtin_prefetch(&dict_data[codes_data[i + 4]], 0, 0);
        __builtin_prefetch(&dict_data[codes_data[i + 5]], 0, 0);
        __builtin_prefetch(&dict_data[codes_data[i + 6]], 0, 0);
        __builtin_prefetch(&dict_data[codes_data[i + 7]], 0, 0);
      }
      
      size_t len0 = s0.size();
      size_t len1 = s1.size();
      size_t len2 = s2.size();
      size_t len3 = s3.size();
      
      unsigned char first0 = len0 ? static_cast<unsigned char>(s0[0]) : 0u;
      unsigned char first1 = len1 ? static_cast<unsigned char>(s1[0]) : 0u;
      unsigned char first2 = len2 ? static_cast<unsigned char>(s2[0]) : 0u;
      unsigned char first3 = len3 ? static_cast<unsigned char>(s3[0]) : 0u;
      
      hash = mix(hash, len0);
      hash = mix(hash, first0);
      hash = mix(hash, len1);
      hash = mix(hash, first1);
      hash = mix(hash, len2);
      hash = mix(hash, first2);
      hash = mix(hash, len3);
      hash = mix(hash, first3);
    }
    
    // Handle remaining codes
    for (; i < codes_size; ++i) {
      const std::string& token = dict_data[codes_data[i]];
      size_t len = token.size();
      hash = mix(hash, len);
      hash = mix(hash, len ? static_cast<unsigned char>(token[0]) : 0u);
    }
  }
  
  return hash;
}