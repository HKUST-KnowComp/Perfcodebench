#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t final_hash = 0;
  const std::size_t n = ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET;
    for (std::size_t i = 0; i < n; ++i) {
      // Serialize id (matches original little-endian write_u32 order)
      uint32_t id = ids[i];
      hash ^= static_cast<uint8_t>(id);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 8);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 16);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 24);
      hash *= FNV_PRIME;

      // Serialize timestamp
      uint32_t t = ts[i];
      hash ^= static_cast<uint8_t>(t);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 8);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 16);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 24);
      hash *= FNV_PRIME;

      // Serialize level
      uint16_t lvl = levels[i];
      hash ^= static_cast<uint8_t>(lvl);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(lvl >> 8);
      hash *= FNV_PRIME;

      // Serialize length
      uint16_t len = lens[i];
      hash ^= static_cast<uint8_t>(len);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(len >> 8);
      hash *= FNV_PRIME;
    }
    final_hash = hash;
  }
  return final_hash;
}