#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Precompute lengths and first bytes for fast access.
  const std::size_t dsize = dict.size();
  std::vector<uint64_t> lengths;
  std::vector<uint64_t> first_bytes;
  lengths.reserve(dsize);
  first_bytes.reserve(dsize);
  for (std::size_t i = 0; i < dsize; ++i) {
    const std::string& s = dict[i];
    lengths.push_back(static_cast<uint64_t>(s.size()));
    first_bytes.push_back(s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0])));
  }

  constexpr uint64_t kInitial = 1469598103934665603ULL;
  constexpr uint64_t kMul = 1099511628211ULL;

  uint64_t hash = kInitial;
  for (uint32_t code : codes) {
    const uint64_t len = lengths[code];
    const uint64_t fb = first_bytes[code];
    hash ^= len;
    hash *= kMul;
    hash ^= fb;
    hash *= kMul;
  }

  return hash;
}
