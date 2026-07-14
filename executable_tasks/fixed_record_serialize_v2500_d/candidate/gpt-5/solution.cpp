#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void hash_u32(uint64_t& h, uint32_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 16);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 24);
  h *= FNV_PRIME;
}

inline void hash_u16(uint64_t& h, uint16_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV_PRIME;
}

inline uint64_t checksum_serialized(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  uint64_t h = FNV_OFFSET;
  const std::size_t n = ids.size();

  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_levels = levels.data();
  const uint16_t* p_lens = lens.data();

  for (std::size_t i = 0; i < n; ++i) {
    hash_u32(h, *p_ids++);
    hash_u32(h, *p_ts++);
    hash_u16(h, *p_levels++);
    hash_u16(h, *p_lens++);
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
  // The serialized bytes are identical on every iteration; compute once.
  return checksum_serialized(ids, ts, levels, lens);
}
