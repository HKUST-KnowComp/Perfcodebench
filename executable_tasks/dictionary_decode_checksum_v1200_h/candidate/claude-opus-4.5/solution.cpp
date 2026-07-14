#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

struct TokenInfo {
  uint64_t len;
  uint64_t first_byte;
};

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
  
  // Pre-compute token info for better cache locality
  const size_t dict_size = dict.size();
  std::vector<TokenInfo> info(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    info[i].len = dict[i].size();
    info[i].first_byte = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }
  
  const TokenInfo* info_ptr = info.data();
  const uint32_t* codes_ptr = codes.data();
  const size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    // Process with prefetching
    constexpr size_t PREFETCH_DIST = 16;
    
    for (; i + PREFETCH_DIST < codes_size; ++i) {
      __builtin_prefetch(&info_ptr[codes_ptr[i + PREFETCH_DIST]], 0, 0);
      const TokenInfo& ti = info_ptr[codes_ptr[i]];
      hash = mix(hash, ti.len);
      hash = mix(hash, ti.first_byte);
    }
    
    // Process remaining elements
    for (; i < codes_size; ++i) {
      const TokenInfo& ti = info_ptr[codes_ptr[i]];
      hash = mix(hash, ti.len);
      hash = mix(hash, ti.first_byte);
    }
  }
  
  return hash;
}