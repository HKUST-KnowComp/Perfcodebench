#include "interface.h"

#include <cstdint>
#include <string>
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
  const std::string* dict_data = dict.data();
  const uint32_t* codes_data = codes.data();
  const std::size_t codes_size = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = codes_size & ~3ULL;
    
    for (; i < unroll_limit; i += 4) {
      const std::string& token0 = dict_data[codes_data[i]];
      const std::string& token1 = dict_data[codes_data[i + 1]];
      const std::string& token2 = dict_data[codes_data[i + 2]];
      const std::string& token3 = dict_data[codes_data[i + 3]];
      
      hash = mix(hash, token0.size());
      hash = mix(hash, token0.empty() ? 0u : static_cast<unsigned char>(token0[0]));
      hash = mix(hash, token1.size());
      hash = mix(hash, token1.empty() ? 0u : static_cast<unsigned char>(token1[0]));
      hash = mix(hash, token2.size());
      hash = mix(hash, token2.empty() ? 0u : static_cast<unsigned char>(token2[0]));
      hash = mix(hash, token3.size());
      hash = mix(hash, token3.empty() ? 0u : static_cast<unsigned char>(token3[0]));
    }
    
    for (; i < codes_size; ++i) {
      const std::string& token = dict_data[codes_data[i]];
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  
  return hash;
}