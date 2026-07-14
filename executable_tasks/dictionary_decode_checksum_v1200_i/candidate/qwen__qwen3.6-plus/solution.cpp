#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const std::string* dict_data = dict.data();
  const uint32_t* codes_data = codes.data();
  const size_t codes_size = codes.size();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < codes_size; ++i) {
      const auto& s = dict_data[codes_data[i]];
      uint64_t len = s.size();
      h ^= len;
      h *= 1099511628211ULL;
      h ^= (len == 0) ? 0u : static_cast<unsigned char>(s[0]);
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}