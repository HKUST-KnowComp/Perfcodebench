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
  std::vector<std::string> decoded;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.clear();
    for (uint32_t code : codes) {
      decoded.push_back(dict[static_cast<std::size_t>(code)]);
    }
    hash = 1469598103934665603ULL;
    for (const std::string& token : decoded) {
      hash = mix(hash, token.size());
      hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
    }
  }
  return hash;
}
