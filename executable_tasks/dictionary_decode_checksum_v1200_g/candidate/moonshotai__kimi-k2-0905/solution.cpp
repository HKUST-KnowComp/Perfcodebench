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
  const std::size_t n = codes.size();
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < n; ++i) {
      const std::string& token = dict[static_cast<std::size_t>(codes[i])];
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  return hash;
}