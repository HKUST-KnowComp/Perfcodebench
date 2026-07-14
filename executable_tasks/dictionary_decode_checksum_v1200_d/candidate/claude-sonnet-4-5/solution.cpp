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
    
    // Unroll by 4 for better ILP
    for (; i + 3 < codes_size; i += 4) {
      uint32_t code0 = codes_data[i];
      uint32_t code1 = codes_data[i + 1];
      uint32_t code2 = codes_data[i + 2];
      uint32_t code3 = codes_data[i + 3];
      
      const std::string& token0 = dict[code0];
      const std::string& token1 = dict[code1];
      const std::string& token2 = dict[code2];
      const std::string& token3 = dict[code3];
      
      // Prefetch next batch
      if (i + 7 < codes_size) {
        __builtin_prefetch(&dict[codes_data[i + 4]], 0, 0);
        __builtin_prefetch(&dict[codes_data[i + 5]], 0, 0);
        __builtin_prefetch(&dict[codes_data[i + 6]], 0, 0);
        __builtin_prefetch(&dict[codes_data[i + 7]], 0, 0);
      }
      
      std::size_t size0 = token0.size();
      std::size_t size1 = token1.size();
      std::size_t size2 = token2.size();
      std::size_t size3 = token3.size();
      
      hash = mix(hash, size0);
      hash = mix(hash, size0 ? static_cast<unsigned char>(token0[0]) : 0u);
      
      hash = mix(hash, size1);
      hash = mix(hash, size1 ? static_cast<unsigned char>(token1[0]) : 0u);
      
      hash = mix(hash, size2);
      hash = mix(hash, size2 ? static_cast<unsigned char>(token2[0]) : 0u);
      
      hash = mix(hash, size3);
      hash = mix(hash, size3 ? static_cast<unsigned char>(token3[0]) : 0u);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      const std::string& token = dict[codes_data[i]];
      std::size_t size = token.size();
      hash = mix(hash, size);
      hash = mix(hash, size ? static_cast<unsigned char>(token[0]) : 0u);
    }
  }
  
  return hash;
}