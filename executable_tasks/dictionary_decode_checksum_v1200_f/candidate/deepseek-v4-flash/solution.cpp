#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t n = codes.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      const std::string& token = dict[codes[i]];
      std::size_t len = token.size();
      hash ^= len;
      hash *= 1099511628211ULL;
      unsigned char first_byte = token.empty() ? 0 : static_cast<unsigned char>(token[0]);
      hash ^= first_byte;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}