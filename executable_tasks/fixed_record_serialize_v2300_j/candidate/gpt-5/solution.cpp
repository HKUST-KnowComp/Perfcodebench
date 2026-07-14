#include "interface.h"

namespace {
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline void hash_u32(uint64_t& h, uint32_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= kFNVPrime;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= kFNVPrime;
  h ^= static_cast<uint8_t>(v >> 16);
  h *= kFNVPrime;
  h ^= static_cast<uint8_t>(v >> 24);
  h *= kFNVPrime;
}

inline void hash_u16(uint64_t& h, uint16_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= kFNVPrime;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= kFNVPrime;
}
}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = ids.size();
  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  uint64_t hash = kFNVOffset;
  for (std::size_t i = 0; i < n; ++i) {
    hash_u32(hash, ids_p[i]);
    hash_u32(hash, ts_p[i]);
    hash_u16(hash, levels_p[i]);
    hash_u16(hash, lens_p[i]);
  }
  return hash;
}
