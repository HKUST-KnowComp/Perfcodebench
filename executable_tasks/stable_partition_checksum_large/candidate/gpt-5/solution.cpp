#include "interface.h"

namespace {
constexpr uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

inline uint64_t hash_stable_partition_once(const uint8_t* flags, const uint32_t* values, std::size_t n) {
  uint64_t h = kFNVOffsetBasis;
  // First pass: all values with flag != 0 in original order
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      h ^= static_cast<uint64_t>(values[i]);
      h *= kFNVPrime;
    }
  }
  // Second pass: all values with flag == 0 in original order
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      h ^= static_cast<uint64_t>(values[i]);
      h *= kFNVPrime;
    }
  }
  return h;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0ULL;
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  return hash_stable_partition_once(f, v, n);
}
