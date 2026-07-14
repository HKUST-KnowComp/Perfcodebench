#include "interface.h"

#include <vector>

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
    int /*iters*/) {
  const std::size_t dict_size = dictionary.size();

  // Aggregate sums per dictionary index (code)
  std::vector<uint64_t> sums(dict_size, 0);
  std::size_t n = codes.size();
  if (values.size() < n) n = values.size();

  const uint16_t* cptr = codes.data();
  const uint32_t* vptr = values.data();
  for (std::size_t i = 0; i < n; ++i) {
    // Assume inputs are valid: codes[i] < dict_size
    sums[static_cast<std::size_t>(cptr[i])] += static_cast<uint64_t>(vptr[i]);
  }

  // Compute checksum in dictionary order
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (std::size_t i = 0; i < dict_size; ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}
