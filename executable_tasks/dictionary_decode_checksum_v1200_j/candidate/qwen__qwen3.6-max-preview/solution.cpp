#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const std::string* dict_ptr = dict.data();
  const uint32_t* codes_ptr = codes.data();
  const uint32_t* codes_end = codes_ptr + codes.size();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const uint32_t* c = codes_ptr; c != codes_end; ++c) {
      const std::string& token = dict_ptr[static_cast<std::size_t>(*c)];
      std::size_t sz = token.size();
      hash ^= static_cast<uint64_t>(sz);
      hash *= 1099511628211ULL;
      hash ^= sz ? static_cast<unsigned char>(token[0]) : 0u;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
