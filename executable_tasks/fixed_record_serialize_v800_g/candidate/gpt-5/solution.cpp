#include "interface.h"

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline uint64_t fnv1a_update_u32(uint64_t h, uint32_t v) {
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

inline uint64_t fnv1a_update_u16(uint64_t h, uint16_t v) {
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
  // Baseline returned 0 when iters == 0
  if (iters <= 0) return 0ULL;

  const std::size_t n = ids.size();
  const uint32_t* pid = ids.data();
  const uint32_t* pts = ts.data();
  const uint16_t* plevels = levels.data();
  const uint16_t* plens = lens.data();

  // Compute checksum of the serialized bytes directly without materializing
  // the byte buffer, preserving the exact little-endian layout.
  uint64_t h = FNV_OFFSET;
  for (std::size_t i = 0; i < n; ++i) {
    h = fnv1a_update_u32(h, pid[i]);
    h = fnv1a_update_u32(h, pts[i]);
    h = fnv1a_update_u16(h, plevels[i]);
    h = fnv1a_update_u16(h, plens[i]);
  }

  return h;
}
