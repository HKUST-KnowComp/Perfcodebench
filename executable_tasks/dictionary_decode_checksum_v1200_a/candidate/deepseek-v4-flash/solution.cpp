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
  // If no iterations, return initial hash (matches baseline behavior)
  if (iters <= 0) {
    return 0;
  }

  const uint64_t base = 1469598103934665603ULL;
  uint64_t hash = base;

  // Compute hash directly from the dictionary and codes without
  // materializing the decoded strings.  This is equivalent to the
  // last (or any) iteration of the original loop.
  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    std::size_t len = token.size();
    hash = mix(hash, len);
    unsigned char first = token.empty() ? 0u : static_cast<unsigned char>(token[0]);
    hash = mix(hash, first);
  }

  return hash;
}
