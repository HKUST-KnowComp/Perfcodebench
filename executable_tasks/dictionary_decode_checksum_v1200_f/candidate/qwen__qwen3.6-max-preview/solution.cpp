#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  uint64_t hash = 0;
  constexpr uint64_t fnv_prime = 1099511628211ULL;
  constexpr uint64_t fnv_offset = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (uint32_t code : codes) {
      const std::string& token = dict[static_cast<std::size_t>(code)];
      hash ^= static_cast<uint64_t>(token.size());
      hash *= fnv_prime;
      hash ^= static_cast<uint64_t>(token.empty() ? 0u : static_cast<unsigned char>(token[0]));
      hash *= fnv_prime;
    }
  }
  return hash;
}
