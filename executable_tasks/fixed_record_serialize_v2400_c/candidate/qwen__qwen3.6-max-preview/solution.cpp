#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_levels = levels.data();
  const uint16_t* p_lens = lens.data();

  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = p_ids[i];
      uint32_t t = p_ts[i];
      uint16_t lvl = p_levels[i];
      uint16_t ln = p_lens[i];

      hash ^= static_cast<uint64_t>(id & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((id >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((id >> 16) & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((id >> 24) & 0xFF); hash *= FNV_PRIME;

      hash ^= static_cast<uint64_t>(t & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((t >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((t >> 16) & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((t >> 24) & 0xFF); hash *= FNV_PRIME;

      hash ^= static_cast<uint64_t>(lvl & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((lvl >> 8) & 0xFF); hash *= FNV_PRIME;

      hash ^= static_cast<uint64_t>(ln & 0xFF); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((ln >> 8) & 0xFF); hash *= FNV_PRIME;
    }
  }
  return hash;
}
