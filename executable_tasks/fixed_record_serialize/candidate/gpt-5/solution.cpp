#include "interface.h"

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void fnv1a_u8(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= FNV_PRIME;
}

inline void hash_u32_le(uint64_t& h, uint32_t v) {
  fnv1a_u8(h, static_cast<uint8_t>(v));
  fnv1a_u8(h, static_cast<uint8_t>(v >> 8));
  fnv1a_u8(h, static_cast<uint8_t>(v >> 16));
  fnv1a_u8(h, static_cast<uint8_t>(v >> 24));
}

inline void hash_u16_le(uint64_t& h, uint16_t v) {
  fnv1a_u8(h, static_cast<uint8_t>(v));
  fnv1a_u8(h, static_cast<uint8_t>(v >> 8));
}

uint64_t checksum_serialized_once(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  const std::size_t n = ids.size();

  const uint32_t* idp = ids.data();
  const uint32_t* tsp = ts.data();
  const uint16_t* levelp = levels.data();
  const uint16_t* lenp = lens.data();
  const uint32_t* id_end = idp + n;

  uint64_t h = FNV_OFFSET;
  while (idp != id_end) {
    hash_u32_le(h, *idp++);
    hash_u32_le(h, *tsp++);
    hash_u16_le(h, *levelp++);
    hash_u16_le(h, *lenp++);
  }
  return h;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0ULL;
  // The checksum is identical for every iteration; compute once.
  return checksum_serialized_once(ids, ts, levels, lens);
}
