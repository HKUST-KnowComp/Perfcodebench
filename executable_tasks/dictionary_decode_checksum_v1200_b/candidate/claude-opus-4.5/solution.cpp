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
  
  const size_t dict_size = dict.size();
  const size_t codes_size = codes.size();
  
  if (codes_size == 0 || iters <= 0) {
    return 1469598103934665603ULL;
  }
  
  // Pre-compute length and first byte for each dictionary entry
  // Pack into a single struct for cache efficiency
  struct DictInfo {
    uint64_t len;
    uint64_t first_byte;
  };
  
  std::vector<DictInfo> dict_info(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    dict_info[i].len = dict[i].size();
    dict_info[i].first_byte = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }
  
  const DictInfo* __restrict info_ptr = dict_info.data();
  const uint32_t* __restrict codes_ptr = codes.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    const size_t unroll_limit = codes_size >= 4 ? codes_size - 3 : 0;
    
    // Unrolled loop for better instruction-level parallelism
    for (; i < unroll_limit; i += 4) {
      // Prefetch ahead
      if (i + 16 < codes_size) {
        __builtin_prefetch(&info_ptr[codes_ptr[i + 16]], 0, 1);
      }
      
      const DictInfo& d0 = info_ptr[codes_ptr[i]];
      const DictInfo& d1 = info_ptr[codes_ptr[i + 1]];
      const DictInfo& d2 = info_ptr[codes_ptr[i + 2]];
      const DictInfo& d3 = info_ptr[codes_ptr[i + 3]];
      
      hash = mix(hash, d0.len);
      hash = mix(hash, d0.first_byte);
      hash = mix(hash, d1.len);
      hash = mix(hash, d1.first_byte);
      hash = mix(hash, d2.len);
      hash = mix(hash, d2.first_byte);
      hash = mix(hash, d3.len);
      hash = mix(hash, d3.first_byte);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      const DictInfo& d = info_ptr[codes_ptr[i]];
      hash = mix(hash, d.len);
      hash = mix(hash, d.first_byte);
    }
  }
  
  return hash;
}