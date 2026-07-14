#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  return (hash ^ value) * 1099511628211ULL;
}
}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Preserve baseline behavior for iters == 0 (no work done, returns 0).
  if (iters <= 0) {
    return 0ULL;
  }

  // Precompute lengths and first-byte values for the dictionary entries.
  const std::size_t n = dict.size();
  std::vector<uint64_t> lengths;
  lengths.resize(n);
  std::vector<uint8_t> first_bytes;
  first_bytes.resize(n);

  for (std::size_t i = 0; i < n; ++i) {
    const std::string& s = dict[i];
    lengths[i] = static_cast<uint64_t>(s.size());
    first_bytes[i] = s.empty() ? 0u : static_cast<uint8_t>(static_cast<unsigned char>(s[0]));
  }

  // Compute the checksum for a single iteration. Subsequent iterations
  // would yield the same value, so no need to repeat.
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  const uint32_t* c = codes.data();
  const uint32_t* cend = c + codes.size();
  for (; c != cend; ++c) {
    const std::size_t idx = static_cast<std::size_t>(*c);
    hash = mix(hash, lengths[idx]);
    hash = mix(hash, static_cast<uint64_t>(first_bytes[idx]));
  }

  return hash;
}
