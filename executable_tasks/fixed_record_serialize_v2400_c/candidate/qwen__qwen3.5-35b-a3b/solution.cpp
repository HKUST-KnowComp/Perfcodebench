#include "interface.h"

#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (std::size_t i = 0; i < ids.size(); ++i) {
      // Process ids[i] (u32) - little endian
      uint32_t id = ids[i];
      hash ^= (id & 0xFF); hash *= FNV_PRIME;
      hash ^= ((id >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= ((id >> 16) & 0xFF); hash *= FNV_PRIME;
      hash ^= ((id >> 24) & 0xFF); hash *= FNV_PRIME;

      // Process ts[i] (u32)
      uint32_t t = ts[i];
      hash ^= (t & 0xFF); hash *= FNV_PRIME;
      hash ^= ((t >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= ((t >> 16) & 0xFF); hash *= FNV_PRIME;
      hash ^= ((t >> 24) & 0xFF); hash *= FNV_PRIME;

      // Process levels[i] (u16)
      uint16_t l = levels[i];
      hash ^= (l & 0xFF); hash *= FNV_PRIME;
      hash ^= ((l >> 8) & 0xFF); hash *= FNV_PRIME;

      // Process lens[i] (u16)
      uint16_t len = lens[i];
      hash ^= (len & 0xFF); hash *= FNV_PRIME;
      hash ^= ((len >> 8) & 0xFF); hash *= FNV_PRIME;
    }
  }
  return hash;
}