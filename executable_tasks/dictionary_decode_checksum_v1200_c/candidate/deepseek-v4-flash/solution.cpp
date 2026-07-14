#include "interface.h"

#include <cstdint>
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
  const uint64_t base = 1469598103934665603ULL;
  const size_t num_codes = codes.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = base;
    for (size_t i = 0; i < num_codes; ++i) {
      const std::string& s = dict[static_cast<size_t>(codes[i])];
      size_t len = s.size();
      hash = mix(hash, len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(s[0]));
    }
  }
  return hash;
}