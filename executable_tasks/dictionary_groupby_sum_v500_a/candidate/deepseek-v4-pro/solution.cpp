#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (iters == 0) {
    return 0;
  }

  std::vector<uint64_t> sums(dictionary.size(), 0);
  for (std::size_t i = 0; i < codes.size(); ++i) {
    sums[codes[i]] += static_cast<uint64_t>(values[i]);
  }

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t idx = 0; idx < dictionary.size(); ++idx) {
    hash = mix(hash, sums[idx]);
  }

  return hash;
}