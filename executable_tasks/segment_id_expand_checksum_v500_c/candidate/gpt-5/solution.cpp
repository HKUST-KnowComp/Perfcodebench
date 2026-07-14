#include "interface.h"

namespace {
inline uint64_t fnv_mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;  // FNV-1a prime (64-bit)
  return h;
}
}

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Match baseline behavior: if iters == 0, return 0 (hash never initialized in baseline)
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = offsets.size();
  // FNV-1a 64-bit offset basis
  uint64_t h = 1469598103934665603ULL;

  // If there are fewer than 2 offsets, there are no expanded items; return offset basis
  if (n <= 1) {
    return h;
  }

  const uint64_t P = 1099511628211ULL;

  // Compute hash directly over the implicit expanded stream once (equivalent to baseline's final iteration)
  for (std::size_t s = 0; s + 1 < n; ++s) {
    uint32_t len = offsets[s + 1] - offsets[s];
    if (len == 0) continue;
    const uint64_t v = static_cast<uint64_t>(s);

    // Unrolled processing for better throughput
    uint32_t m = len;
    while (m >= 8) {
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      h ^= v; h *= P;
      m -= 8;
    }
    switch (m) {
      case 7: h ^= v; h *= P; [[fallthrough]];
      case 6: h ^= v; h *= P; [[fallthrough]];
      case 5: h ^= v; h *= P; [[fallthrough]];
      case 4: h ^= v; h *= P; [[fallthrough]];
      case 3: h ^= v; h *= P; [[fallthrough]];
      case 2: h ^= v; h *= P; [[fallthrough]];
      case 1: h ^= v; h *= P; [[fallthrough]];
      default: break;
    }
  }

  return h;
}
