#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Preserve baseline behavior: if iters == 0, the loop never runs and hash remains 0
  if (iters == 0) {
    return 0ULL;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    hash = mix(hash, static_cast<uint64_t>(token.size()));
    uint64_t lead = token.empty() ? 0ULL : static_cast<unsigned char>(token[0]);
    hash = mix(hash, lead);
  }

  return hash;
}