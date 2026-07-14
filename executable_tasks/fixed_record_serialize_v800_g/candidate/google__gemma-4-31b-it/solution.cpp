#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t final_hash = 0;
  size_t n = ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET_BASIS;
    
    for (size_t i = 0; i < n; ++i) {
      // Serialize ids[i] (u32)
      uint32_t val_id = ids[i];
      hash ^= static_cast<uint64_t>(val_id & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_id >> 8) & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_id >> 16) & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_id >> 24) & 0xFF);
      hash *= FNV_PRIME;

      // Serialize ts[i] (u32)
      uint32_t val_ts = ts[i];
      hash ^= static_cast<uint64_t>(val_ts & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_ts >> 8) & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_ts >> 16) & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_ts >> 24) & 0xFF);
      hash *= FNV_PRIME;

      // Serialize levels[i] (u16)
      uint16_t val_lvl = levels[i];
      hash ^= static_cast<uint64_t>(val_lvl & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_lvl >> 8) & 0xFF);
      hash *= FNV_PRIME;

      // Serialize lens[i] (u16)
      uint16_t val_len = lens[i];
      hash ^= static_cast<uint64_t>(val_len & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((val_len >> 8) & 0xFF);
      hash *= FNV_PRIME;
    }
    final_hash = hash;
  }
  
  return final_hash;
}