#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0ULL;  // Match baseline behavior

  constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;
  const uint32_t* p = codes.data();
  const uint32_t* e = p + codes.size();

  for (; p != e; ++p) {
    const std::string& s = dict[static_cast<std::size_t>(*p)];
    const std::size_t len = s.size();

    hash ^= static_cast<uint64_t>(len);
    hash *= FNV_PRIME;

    const uint64_t lead = (len == 0) ? 0u : static_cast<unsigned char>(s[0]);
    hash ^= lead;
    hash *= FNV_PRIME;
  }

  return hash;
}
