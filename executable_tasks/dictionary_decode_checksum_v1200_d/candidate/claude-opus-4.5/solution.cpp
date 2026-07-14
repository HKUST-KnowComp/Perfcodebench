#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  const std::string* const __restrict dict_data = dict.data();
  const uint32_t* const __restrict codes_data = codes.data();
  const std::size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = codes_size & ~static_cast<std::size_t>(3);
    
    // Unrolled loop processing 4 codes at a time
    for (; i < unroll_limit; i += 4) {
      // Prefetch ahead
      if (i + 16 < codes_size) {
        __builtin_prefetch(&dict_data[codes_data[i + 16]], 0, 0);
      }
      
      const std::string& s0 = dict_data[codes_data[i]];
      const std::string& s1 = dict_data[codes_data[i + 1]];
      const std::string& s2 = dict_data[codes_data[i + 2]];
      const std::string& s3 = dict_data[codes_data[i + 3]];
      
      const std::size_t len0 = s0.size();
      const std::size_t len1 = s1.size();
      const std::size_t len2 = s2.size();
      const std::size_t len3 = s3.size();
      
      const uint64_t c0 = __builtin_expect(len0 != 0, 1) ? static_cast<unsigned char>(s0[0]) : 0u;
      const uint64_t c1 = __builtin_expect(len1 != 0, 1) ? static_cast<unsigned char>(s1[0]) : 0u;
      const uint64_t c2 = __builtin_expect(len2 != 0, 1) ? static_cast<unsigned char>(s2[0]) : 0u;
      const uint64_t c3 = __builtin_expect(len3 != 0, 1) ? static_cast<unsigned char>(s3[0]) : 0u;
      
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
      const std::size_t len = token.size();
      hash = mix(hash, len);
      hash = mix(hash, __builtin_expect(len != 0, 1) ? static_cast<unsigned char>(token[0]) : 0u);
    }
  }
  
  return hash;
}