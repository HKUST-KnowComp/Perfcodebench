#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void fnv1_update_u32(uint64_t& h, uint32_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 16);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 24);
  h *= FNV_PRIME;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  uint64_t hash = 0;
  const std::size_t n = ids.size();

  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;
    for (std::size_t i = 0; i < n; ++i) {
      fnv1_update_u32(h, ids_p[i]);
      fnv1_update_u32(h, ts_p[i]);
      uint32_t lv_ln = static_cast<uint32_t>(levels_p[i]) |
                       (static_cast<uint32_t>(lens_p[i]) << 16);
      fnv1_update_u32(h, lv_ln);
    }
    hash = h;
  }

  return hash;
}
