#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::string& token = dict[static_cast<std::size_t>(code)];
      hash ^= token.size();
      hash *= 1099511628211ULL;
      hash ^= token.empty() ? 0u : static_cast<unsigned char>(token[0]);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
