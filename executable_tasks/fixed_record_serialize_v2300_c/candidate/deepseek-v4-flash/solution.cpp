#include "interface.h"

#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

  if (iters <= 0) return 0;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;
  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    // id
    uint32_t v = ids[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 8));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 16));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 24));
    hash *= FNV_PRIME;
    // ts
    v = ts[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 8));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 16));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> 24));
    hash *= FNV_PRIME;
    // level
    uint16_t w = levels[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(w));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(w >> 8));
    hash *= FNV_PRIME;
    // len
    w = lens[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(w));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(w >> 8));
    hash *= FNV_PRIME;
  }

  return hash;
}