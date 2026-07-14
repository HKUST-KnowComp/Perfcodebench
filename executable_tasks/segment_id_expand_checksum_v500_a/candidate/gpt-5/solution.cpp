#include "interface.h"

#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t mix_repeat(uint64_t h, uint64_t v, uint32_t count) {
  uint32_t c = count;
  while (c >= 8) {
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    h ^= v; h *= FNV_PRIME;
    c -= 8;
  }
  while (c--) {
    h ^= v;
    h *= FNV_PRIME;
  }
  return h;
}
}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  uint64_t hash = FNV_OFFSET_BASIS;
  const std::size_t n = offsets.size();

  for (std::size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];  // uint32_t wrap semantics
    const uint64_t val = static_cast<uint64_t>(static_cast<uint32_t>(s));
    hash = mix_repeat(hash, val, len);
  }

  return hash;
}
