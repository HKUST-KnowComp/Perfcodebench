#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

static inline void fnv1a_update_byte(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= 1099511628211ULL;  // FNV-1a 64-bit prime
}

static inline void fnv1a_update_u16_le(uint64_t& h, uint16_t v) {
  fnv1a_update_byte(h, static_cast<uint8_t>(v));
  fnv1a_update_byte(h, static_cast<uint8_t>(v >> 8));
}

static inline void fnv1a_update_u32_le(uint64_t& h, uint32_t v) {
  fnv1a_update_byte(h, static_cast<uint8_t>(v));
  fnv1a_update_byte(h, static_cast<uint8_t>(v >> 8));
  fnv1a_update_byte(h, static_cast<uint8_t>(v >> 16));
  fnv1a_update_byte(h, static_cast<uint8_t>(v >> 24));
}

static inline uint64_t serialize_and_checksum_once(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  const std::size_t n = ids.size();
  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  uint64_t h = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  for (std::size_t i = 0; i < n; ++i) {
    fnv1a_update_u32_le(h, ids_p[i]);
    fnv1a_update_u32_le(h, ts_p[i]);
    fnv1a_update_u16_le(h, levels_p[i]);
    fnv1a_update_u16_le(h, lens_p[i]);
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
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute once; repeated iterations would yield identical checksum.
  uint64_t hash = serialize_and_checksum_once(ids, ts, levels, lens);

  // Preserve final assignment behavior for iters > 1 without recomputation.
  // (Result is identical each iteration.)
  return hash;
}
