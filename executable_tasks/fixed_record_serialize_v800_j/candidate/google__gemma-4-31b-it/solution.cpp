#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t hash = 0;
  size_t n = ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = FNV_OFFSET_BASIS;
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t lvl = levels[i];
      uint16_t len = lens[i];

      // Serialize id (u32)
      current_hash ^= static_cast<uint64_t>(id & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((id >> 8) & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((id >> 16) & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((id >> 24) & 0xFF);
      current_hash *= FNV_PRIME;

      // Serialize ts (u32)
      current_hash ^= static_cast<uint64_t>(t & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((t >> 8) & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((t >> 16) & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((t >> 24) & 0xFF);
      current_hash *= FNV_PRIME;

      // Serialize levels (u16)
      current_hash ^= static_cast<uint64_t>(lvl & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((lvl >> 8) & 0xFF);
      current_hash *= FNV_PRIME;

      // Serialize lens (u16)
      current_hash ^= static_cast<uint64_t>(len & 0xFF);
      current_hash *= FNV_PRIME;
      current_hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
      current_hash *= FNV_PRIME;
    }
    hash = current_hash;
  }
  return hash;
}