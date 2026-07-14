#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Accumulate sums per dictionary index (code) directly.
  std::vector<uint64_t> sums(dict_size);

  const uint16_t* cptr = codes.empty() ? nullptr : codes.data();
  const uint32_t* vptr = values.empty() ? nullptr : values.data();

  for (std::size_t i = 0; i < n; ++i) {
    // Assuming valid codes within [0, dict_size). Baseline would crash on invalid codes via dictionary access.
    sums[static_cast<std::size_t>(cptr[i])] += static_cast<uint64_t>(vptr[i]);
  }

  // Compute checksum (FNV-like mixing) in dictionary order
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < dict_size; ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}
