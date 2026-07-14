#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline void fnv1a_u8(uint64_t& hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
}

static inline void fnv1a_u16_le(uint64_t& hash, uint16_t v) {
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v));
  hash *= 1099511628211ULL;
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 8));
  hash *= 1099511628211ULL;
}

static inline void fnv1a_u32_le(uint64_t& hash, uint32_t v) {
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v));
  hash *= 1099511628211ULL;
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 8));
  hash *= 1099511628211ULL;
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 16));
  hash *= 1099511628211ULL;
  hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 24));
  hash *= 1099511628211ULL;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  if (iters <= 0) return 0;

  const uint32_t* idp = ids.data();
  const uint32_t* tsp = ts.data();
  const uint16_t* levelp = levels.data();
  const uint16_t* lenp = lens.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      fnv1a_u32_le(h, idp[i]);
      fnv1a_u32_le(h, tsp[i]);
      fnv1a_u16_le(h, levelp[i]);
      fnv1a_u16_le(h, lenp[i]);
    }
    hash = h;
  }
  return hash;
}
