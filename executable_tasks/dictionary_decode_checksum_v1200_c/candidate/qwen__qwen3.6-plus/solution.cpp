#include "interface.h"

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (uint32_t code : codes) {
    const std::string& s = dict[code];
    hash ^= s.size();
    hash *= prime;
    hash ^= s.empty() ? 0 : static_cast<uint8_t>(s[0]);
    hash *= prime;
  }

  return hash;
}
