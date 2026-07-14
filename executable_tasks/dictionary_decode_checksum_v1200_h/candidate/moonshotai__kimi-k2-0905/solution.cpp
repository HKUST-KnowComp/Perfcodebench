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
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::string& token = dict[static_cast<size_t>(code)];
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  return hash;
}