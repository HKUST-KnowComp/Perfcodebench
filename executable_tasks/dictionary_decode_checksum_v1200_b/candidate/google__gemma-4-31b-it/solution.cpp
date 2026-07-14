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
  const size_t num_codes = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_codes; ++i) {
      const std::string& token = dict[codes[i]];
      hash = mix(hash, static_cast<uint64_t>(token.size()));
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  return hash;
}