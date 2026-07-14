#include "interface.h"

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
  // Preserve baseline behavior for iters == 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  const uint8_t* f = flags.data();
  const uint8_t* fend = f + n;
  const uint32_t* v = values.data();

  // First pass: elements with flag true
  const uint8_t* f1 = f;
  const uint32_t* v1 = v;
  for (; f1 != fend; ++f1, ++v1) {
    if (*f1) {
      hash = mix(hash, static_cast<uint64_t>(*v1));
    }
  }

  // Second pass: elements with flag false
  const uint8_t* f2 = f;
  const uint32_t* v2 = v;
  for (; f2 != fend; ++f2, ++v2) {
    if (!*f2) {
      hash = mix(hash, static_cast<uint64_t>(*v2));
    }
  }

  return hash;
}
