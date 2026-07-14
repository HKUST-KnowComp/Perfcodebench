#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_u32(uint64_t h, uint32_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 16);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 24);
  h *= FNV_PRIME;
  return h;
}

inline uint64_t fnv1a_u16(uint64_t h, uint16_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV_PRIME;
  return h;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET_BASIS;

    const uint32_t* ids_p = ids.data();
    const uint32_t* ts_p = ts.data();
    const uint16_t* lv_p = levels.data();
    const uint16_t* ln_p = lens.data();

    for (std::size_t i = 0; i < n; ++i) {
      h = fnv1a_u32(h, ids_p[i]);
      h = fnv1a_u32(h, ts_p[i]);
      h = fnv1a_u16(h, lv_p[i]);
      h = fnv1a_u16(h, ln_p[i]);
    }

    hash = h;  // match baseline: use checksum from the last iteration
  }

  return hash;
}
