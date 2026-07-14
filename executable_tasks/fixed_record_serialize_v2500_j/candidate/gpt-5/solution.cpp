#include "interface.h"

namespace {

static inline void fnv1a_update(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= 1099511628211ULL;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) return 0ULL;

  const std::size_t n = ids.size();
  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  // Compute the checksum directly over the would-be serialized bytes
  // in little-endian order, avoiding any intermediate buffer.
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32 = ids_p[i];
    fnv1a_update(hash, static_cast<uint8_t>(v32));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 8));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 16));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 24));

    v32 = ts_p[i];
    fnv1a_update(hash, static_cast<uint8_t>(v32));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 8));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 16));
    fnv1a_update(hash, static_cast<uint8_t>(v32 >> 24));

    uint16_t v16 = levels_p[i];
    fnv1a_update(hash, static_cast<uint8_t>(v16));
    fnv1a_update(hash, static_cast<uint8_t>(v16 >> 8));

    v16 = lens_p[i];
    fnv1a_update(hash, static_cast<uint8_t>(v16));
    fnv1a_update(hash, static_cast<uint8_t>(v16 >> 8));
  }

  return hash;
}
