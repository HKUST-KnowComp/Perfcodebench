#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Match baseline behavior for non-positive iterations
  if (iters <= 0) return 0ULL;

  constexpr uint64_t kInit = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t hash = kInit;

  const std::size_t n = offsets.size();
  if (n >= 2) {
    const uint32_t* data = offsets.data();
    const std::size_t s_count = n - 1;

    for (std::size_t s = 0; s < s_count; ++s) {
      const uint32_t len = data[s + 1] - data[s];
      const uint64_t v = static_cast<uint64_t>(s);

      uint32_t blocks = len / 8u;
      while (blocks--) {
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
        hash ^= v; hash *= kPrime;
      }
      switch (len & 7u) {
        case 7: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 6: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 5: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 4: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 3: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 2: hash ^= v; hash *= kPrime; [[fallthrough]];
        case 1: hash ^= v; hash *= kPrime; [[fallthrough]];
        default: break;
      }
    }
  }

  return hash;
}
