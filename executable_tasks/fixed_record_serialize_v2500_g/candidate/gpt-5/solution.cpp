#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_update(uint64_t h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= FNV_PRIME;
  return h;
}

inline uint64_t hash_u16_le(uint64_t h, uint16_t v) {
  h = fnv1a_update(h, static_cast<uint8_t>(v));
  h = fnv1a_update(h, static_cast<uint8_t>(v >> 8));
  return h;
}

inline uint64_t hash_u32_le(uint64_t h, uint32_t v) {
  h = fnv1a_update(h, static_cast<uint8_t>(v));
  h = fnv1a_update(h, static_cast<uint8_t>(v >> 8));
  h = fnv1a_update(h, static_cast<uint8_t>(v >> 16));
  h = fnv1a_update(h, static_cast<uint8_t>(v >> 24));
  return h;
}
}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }
  const std::size_t n = ids.size();
  uint64_t hash = FNV_OFFSET_BASIS;
  for (std::size_t i = 0; i < n; ++i) {
    hash = hash_u32_le(hash, ids[i]);
    hash = hash_u32_le(hash, ts[i]);
    hash = hash_u16_le(hash, levels[i]);
    hash = hash_u16_le(hash, lens[i]);
  }
  // All iterations serialize identically; final checksum equals this value.
  return hash;
}
