#include "interface.h"

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
  const std::size_t codes_size = codes.size();
  const uint32_t* codes_data = codes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < codes_size; ++i) {
      const std::string& token = dict[codes_data[i]];
      const std::size_t len = token.size();
      hash = mix(hash, len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  return hash;
}