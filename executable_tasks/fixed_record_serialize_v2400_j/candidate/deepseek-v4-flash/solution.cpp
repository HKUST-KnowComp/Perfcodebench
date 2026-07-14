#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters == 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    // Serialize id (uint32_t, little endian) and update hash
    uint32_t v = ids[i];
    hash ^= static_cast<uint8_t>(v);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 8);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 16);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 24);
    hash *= 1099511628211ULL;

    // Serialize ts
    v = ts[i];
    hash ^= static_cast<uint8_t>(v);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 8);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 16);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 24);
    hash *= 1099511628211ULL;

    // Serialize level (uint16_t, little endian)
    uint16_t w = levels[i];
    hash ^= static_cast<uint8_t>(w);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(w >> 8);
    hash *= 1099511628211ULL;

    // Serialize lens
    w = lens[i];
    hash ^= static_cast<uint8_t>(w);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(w >> 8);
    hash *= 1099511628211ULL;
  }
  return hash;
}
