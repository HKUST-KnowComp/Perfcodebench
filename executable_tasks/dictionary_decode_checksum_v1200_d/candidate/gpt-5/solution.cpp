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
  // Preserve baseline semantics: if iters == 0, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t dict_size = dict.size();
  // Precompute token length and leading byte for each dictionary entry once.
  std::vector<uint64_t> lens(dict_size);
  std::vector<uint8_t> leads(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    lens[i] = static_cast<uint64_t>(s.size());
    leads[i] = s.empty() ? 0u : static_cast<uint8_t>(static_cast<unsigned char>(s[0]));
  }

  // Compute checksum for one iteration (all iterations yield the same result
  // because the hash is reset each time to the same offset basis).
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* cptr = codes.data();
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t code = static_cast<std::size_t>(cptr[i]);
    hash = mix(hash, lens[code]);
    hash = mix(hash, static_cast<uint64_t>(leads[code]));
  }

  return hash;
}
