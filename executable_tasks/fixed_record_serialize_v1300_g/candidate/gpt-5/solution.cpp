#include "interface.h"

namespace {
constexpr uint64_t FNV64_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV64_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_mix_u32(uint64_t h, uint32_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV64_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV64_PRIME;
  h ^= static_cast<uint8_t>(v >> 16);
  h *= FNV64_PRIME;
  h ^= static_cast<uint8_t>(v >> 24);
  h *= FNV64_PRIME;
  return h;
}

inline uint64_t fnv1a_mix_u16(uint64_t h, uint16_t v) {
  h ^= static_cast<uint8_t>(v);
  h *= FNV64_PRIME;
  h ^= static_cast<uint8_t>(v >> 8);
  h *= FNV64_PRIME;
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
  uint64_t last_hash = 0;

  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV64_OFFSET_BASIS;

    const uint32_t* pid = ids_p;
    const uint32_t* pts = ts_p;
    const uint16_t* plv = levels_p;
    const uint16_t* pln = lens_p;

    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = *pid++;
      uint32_t t = *pts++;
      uint16_t lv = *plv++;
      uint16_t ln = *pln++;

      h = fnv1a_mix_u32(h, id);
      h = fnv1a_mix_u32(h, t);
      h = fnv1a_mix_u16(h, lv);
      h = fnv1a_mix_u16(h, ln);
    }

    last_hash = h;
  }

  return last_hash;
}
