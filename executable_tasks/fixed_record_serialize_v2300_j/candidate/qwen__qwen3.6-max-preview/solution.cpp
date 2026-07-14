#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  uint64_t final_hash = 0;
  const std::size_t n = ids.size();
  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_levels = levels.data();
  const uint16_t* p_lens = lens.data();
  
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = p_ids[i];
      uint32_t t = p_ts[i];
      uint16_t lev = p_levels[i];
      uint16_t ln = p_lens[i];

      h ^= static_cast<uint8_t>(id); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(id >> 8); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(id >> 16); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(id >> 24); h *= FNV_PRIME;

      h ^= static_cast<uint8_t>(t); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(t >> 8); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(t >> 16); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(t >> 24); h *= FNV_PRIME;

      h ^= static_cast<uint8_t>(lev); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(lev >> 8); h *= FNV_PRIME;

      h ^= static_cast<uint8_t>(ln); h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(ln >> 8); h *= FNV_PRIME;
    }
    final_hash = h;
  }
  return final_hash;
}
