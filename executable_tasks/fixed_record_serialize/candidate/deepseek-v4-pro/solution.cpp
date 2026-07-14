#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
  // Checksum constants
  constexpr uint64_t INIT_HASH = 1469598103934665603ULL;
  constexpr uint64_t HASH_MULT = 1099511628211ULL;
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters)
{
  // If no iterations, return 0 as in the baseline.
  if (iters <= 0) return 0ULL;

  uint64_t hash = INIT_HASH;
  const std::size_t n = ids.size(); // assume all vectors same size
  
  // Process each record's bytes in the exact serialization order.
  for (std::size_t i = 0; i < n; ++i) {
    // id: uint32 Little Endian
    uint32_t id = ids[i];
    hash ^= static_cast<uint64_t>(id & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((id >> 8) & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((id >> 16) & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((id >> 24) & 0xFF); hash *= HASH_MULT;

    // ts: uint32 Little Endian
    uint32_t timestamp = ts[i];
    hash ^= static_cast<uint64_t>(timestamp & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((timestamp >> 8) & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((timestamp >> 16) & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((timestamp >> 24) & 0xFF); hash *= HASH_MULT;

    // level: uint16 Little Endian
    uint16_t level = levels[i];
    hash ^= static_cast<uint64_t>(level & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((level >> 8) & 0xFF); hash *= HASH_MULT;

    // lens: uint16 Little Endian
    uint16_t len = lens[i];
    hash ^= static_cast<uint64_t>(len & 0xFF); hash *= HASH_MULT;
    hash ^= static_cast<uint64_t>((len >> 8) & 0xFF); hash *= HASH_MULT;
  }

  return hash;
}
