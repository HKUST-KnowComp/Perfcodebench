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
  
  const size_t dict_size = dict.size();
  const size_t codes_size = codes.size();
  
  // Pre-compute lengths and first bytes for better cache locality
  std::vector<uint64_t> lengths(dict_size);
  std::vector<uint64_t> first_bytes(dict_size);
  
  for (size_t i = 0; i < dict_size; ++i) {
    lengths[i] = dict[i].size();
    first_bytes[i] = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }
  
  const uint64_t* __restrict len_ptr = lengths.data();
  const uint64_t* __restrict fb_ptr = first_bytes.data();
  const uint32_t* __restrict code_ptr = codes.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    const size_t unroll_limit = codes_size & ~size_t(3);
    
    // Unrolled loop - process 4 codes at a time
    for (; i < unroll_limit; i += 4) {
      uint32_t c0 = code_ptr[i];
      uint32_t c1 = code_ptr[i + 1];
      uint32_t c2 = code_ptr[i + 2];
      uint32_t c3 = code_ptr[i + 3];
      
      hash = mix(hash, len_ptr[c0]);
      hash = mix(hash, fb_ptr[c0]);
      hash = mix(hash, len_ptr[c1]);
      hash = mix(hash, fb_ptr[c1]);
      hash = mix(hash, len_ptr[c2]);
      hash = mix(hash, fb_ptr[c2]);
      hash = mix(hash, len_ptr[c3]);
      hash = mix(hash, fb_ptr[c3]);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      uint32_t c = code_ptr[i];
      hash = mix(hash, len_ptr[c]);
      hash = mix(hash, fb_ptr[c]);
    }
  }
  
  return hash;
}