#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  struct Meta {
    uint64_t len;
    uint64_t first;
  };

  const size_t dict_size = dict.size();
  std::vector<Meta> meta(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    meta[i].len = dict[i].size();
    meta[i].first = dict[i].empty() ? 0 : static_cast<unsigned char>(dict[i][0]);
  }

  uint64_t hash = 0;
  const uint32_t* codes_ptr = codes.data();
  const size_t codes_count = codes.size();
  const Meta* meta_ptr = meta.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < codes_count; ++i) {
      const Meta& m = meta_ptr[codes_ptr[i]];
      
      hash ^= m.len;
      hash *= 1099511628211ULL;
      
      hash ^= m.first;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}