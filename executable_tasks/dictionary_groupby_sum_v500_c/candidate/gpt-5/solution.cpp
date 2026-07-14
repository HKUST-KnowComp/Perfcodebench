#include "interface.h"

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
  // Match baseline behavior: if no iterations, return initial hash (0)
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0ULL);

  const std::size_t n = codes.size();
  const uint16_t* cptr = codes.data();
  const uint32_t* vptr = values.data();

  // Aggregate values by code directly into sums indexed by dictionary index
  for (std::size_t i = 0; i < n; ++i) {
    sums[static_cast<std::size_t>(cptr[i])] += static_cast<uint64_t>(vptr[i]);
  }

  // Compute checksum in dictionary order
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  for (std::size_t i = 0; i < dict_size; ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}
