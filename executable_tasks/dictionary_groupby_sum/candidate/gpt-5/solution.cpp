#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV 64-bit prime
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int /*iters*/) {
  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Aggregate sums per dictionary index
  std::vector<uint64_t> sums(dict_size, 0ULL);
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(codes[i]);
    // Assuming codes are valid indices into dictionary as per task contract.
    sums[idx] += static_cast<uint64_t>(values[i]);
  }

  // Compute checksum in dictionary order
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  for (std::size_t i = 0; i < dict_size; ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}
