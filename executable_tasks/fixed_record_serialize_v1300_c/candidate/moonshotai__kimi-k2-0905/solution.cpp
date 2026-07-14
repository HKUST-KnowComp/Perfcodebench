#include "interface.h"
#include <cstddef>
#include <cstdint>

namespace {
inline uint64_t hash_u32(uint64_t hash, uint32_t v) {
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  hash ^= (v & 0xFFU);
  hash *= FNV_PRIME;
  hash ^= ((v >> 8) & 0xFFU);
  hash *= FNV_PRIME;
  hash ^= ((v >> 16) & 0xFFU);
  hash *= FNV_PRIME;
  hash ^= ((v >> 24) & 0xFFU);
  hash *= FNV_PRIME;
  return hash;
}

inline uint64_t hash_u16(uint64_t hash, uint16_t v) {
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  hash ^= (v & 0xFFU);
  hash *= FNV_PRIME;
  hash ^= ((v >> 8) & 0xFFU);
  hash *= FNV_PRIME;
  return hash;
}
}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = hash;
    for (std::size_t i = 0; i < n; ++i) {
      h = hash_u32(h, ids[i]);
      h = hash_u32(h, ts[i]);
      h = hash_u16(h, levels[i]);
      h = hash_u16(h, lens[i]);
    }
    hash = h;
  }
  return hash;
}