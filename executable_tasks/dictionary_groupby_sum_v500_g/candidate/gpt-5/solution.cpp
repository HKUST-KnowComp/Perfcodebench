#include "interface.h"

#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV-1a prime
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Aggregate directly by code (which indexes the dictionary)
  std::vector<uint64_t> sums(dict_size, 0ULL);
  const uint16_t* cp = codes.data();
  const uint32_t* vp = values.data();
  uint64_t* sp = sums.data();

  std::size_t i = 0;
  const std::size_t n8 = n & ~static_cast<std::size_t>(7);
  for (; i < n8; i += 8) {
    sp[cp[i + 0]] += static_cast<uint64_t>(vp[i + 0]);
    sp[cp[i + 1]] += static_cast<uint64_t>(vp[i + 1]);
    sp[cp[i + 2]] += static_cast<uint64_t>(vp[i + 2]);
    sp[cp[i + 3]] += static_cast<uint64_t>(vp[i + 3]);
    sp[cp[i + 4]] += static_cast<uint64_t>(vp[i + 4]);
    sp[cp[i + 5]] += static_cast<uint64_t>(vp[i + 5]);
    sp[cp[i + 6]] += static_cast<uint64_t>(vp[i + 6]);
    sp[cp[i + 7]] += static_cast<uint64_t>(vp[i + 7]);
  }
  for (; i < n; ++i) {
    sp[cp[i]] += static_cast<uint64_t>(vp[i]);
  }

  // Compute checksum in dictionary order
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  for (std::size_t j = 0; j < dict_size; ++j) {
    hash = mix(hash, sp[j]);
  }

  return hash;
}
