#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  const std::size_t n = values.size();
  const uint8_t* flag_ptr = flags.data();
  const uint32_t* val_ptr = values.data();
  uint64_t hash = 1469598103934665603ULL;
  // first pass: elements with flag true
  for (std::size_t i = 0; i < n; ++i) {
    if (flag_ptr[i]) {
      hash = mix(hash, val_ptr[i]);
    }
  }
  // second pass: elements with flag false
  for (std::size_t i = 0; i < n; ++i) {
    if (!flag_ptr[i]) {
      hash = mix(hash, val_ptr[i]);
    }
  }
  return hash;
}