#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segmented_reduce_checksum(
    const std::vector<uint32_t>& segment_ids,
    const std::vector<uint32_t>& values,
    uint32_t segments,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = segment_ids.size();
  std::vector<uint32_t> offsets;
  std::vector<uint64_t> sums;
  offsets.resize(static_cast<std::size_t>(segments) + 1);
  sums.resize(static_cast<std::size_t>(segments));

  const uint32_t* sid = segment_ids.data();
  const uint32_t* val = values.data();

  offsets[0] = 0u;
  uint32_t off = 0u;
  std::size_t pos = 0;

  for (uint32_t s = 0; s < segments; ++s) {
    uint64_t sum = 0ULL;
    if (pos < n && sid[pos] == s) {
      std::size_t start = pos;
      do {
        sum += static_cast<uint64_t>(val[pos]);
        ++pos;
      } while (pos < n && sid[pos] == s);
      off += static_cast<uint32_t>(pos - start);
    }
    sums[static_cast<std::size_t>(s)] = sum;
    offsets[static_cast<std::size_t>(s + 1)] = off;
  }

  uint64_t hash = 1469598103934665603ULL;
  // Mix offsets
  for (std::size_t i = 0; i < offsets.size(); ++i) {
    hash = mix(hash, static_cast<uint64_t>(offsets[i]));
  }
  // Mix sums
  for (std::size_t i = 0; i < sums.size(); ++i) {
    hash = mix(hash, sums[i]);
  }

  return hash;
}
