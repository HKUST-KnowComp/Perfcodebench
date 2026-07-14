#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t fnv_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  uint64_t hash = fnv_basis;
  const std::size_t n = offsets.size();
  if (n < 2) {
    return hash;
  }

  const uint32_t* off = offsets.data();
  for (std::size_t i = 0; i + 1 < n; ++i) {
    // Compute length as uint32_t with wrap-around semantics identical to baseline
    uint32_t len = static_cast<uint32_t>(off[i + 1] - off[i]);
    const uint64_t sv = static_cast<uint64_t>(static_cast<uint32_t>(i));

    // Unroll by 8 for better throughput
    while (len >= 8) {
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      hash ^= sv; hash *= fnv_prime;
      len -= 8;
    }

    switch (len) {
      case 7: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 6: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 5: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 4: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 3: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 2: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      case 1: hash ^= sv; hash *= fnv_prime; [[fallthrough]];
      default: break;
    }
  }

  return hash;
}
