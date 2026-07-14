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
  
  // Pre-calculate the size of the codes vector to avoid repeated calls
  const size_t num_codes = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_codes; ++i) {
      const std::string& token = dict[codes[i]];
      
      // Mix the length of the token
      hash = mix(hash, static_cast<uint64_t>(token.size()));
      
      // Mix the first byte of the token if it exists
      if (!token.empty()) {
        hash = mix(hash, static_cast<unsigned char>(token[0]));
      } else {
        hash = mix(hash, 0u);
      }
    }
  }
  return hash;
}