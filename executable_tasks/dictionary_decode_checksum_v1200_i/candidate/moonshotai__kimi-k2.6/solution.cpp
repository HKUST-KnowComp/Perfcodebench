#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= FNV_PRIME;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters == 0) {
    return 0;
  }

  uint64_t hash = FNV_OFFSET_BASIS;
  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    hash = mix(hash, token.size());
    hash = mix(hash, token.empty() ? 0u : static_cast<unsigned char>(token[0]));
  }
  return hash;
}
