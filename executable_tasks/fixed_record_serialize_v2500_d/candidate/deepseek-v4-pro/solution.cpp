#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int /*iters*/) {
  const uint64_t HASH_START = 1469598103934665603ULL;
  const uint64_t HASH_MUL   = 1099511628211ULL;
  uint64_t hash = HASH_START;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    // id (u32 little-endian)
    {
      uint32_t v = ids[i];
      hash ^= static_cast<uint8_t>(v);       hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 8);  hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 16); hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 24); hash *= HASH_MUL;
    }
    // ts (u32 little-endian)
    {
      uint32_t v = ts[i];
      hash ^= static_cast<uint8_t>(v);       hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 8);  hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 16); hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 24); hash *= HASH_MUL;
    }
    // level (u16 little-endian)
    {
      uint16_t v = levels[i];
      hash ^= static_cast<uint8_t>(v);       hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 8);  hash *= HASH_MUL;
    }
    // lens (u16 little-endian)
    {
      uint16_t v = lens[i];
      hash ^= static_cast<uint8_t>(v);       hash *= HASH_MUL;
      hash ^= static_cast<uint8_t>(v >> 8);  hash *= HASH_MUL;
    }
  }
  return hash;
}