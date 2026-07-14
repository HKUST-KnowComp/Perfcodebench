#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  const size_t n = ids.size();

  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_lvl = levels.data();
  const uint16_t* p_ln = lens.data();

  for (size_t i = 0; i < n; ++i) {
      uint32_t id = p_ids[i];
      uint32_t t = p_ts[i];
      uint16_t lvl = p_lvl[i];
      uint16_t ln = p_ln[i];

      hash ^= (id & 0xFF); hash *= prime;
      hash ^= ((id >> 8) & 0xFF); hash *= prime;
      hash ^= ((id >> 16) & 0xFF); hash *= prime;
      hash ^= (id >> 24); hash *= prime;

      hash ^= (t & 0xFF); hash *= prime;
      hash ^= ((t >> 8) & 0xFF); hash *= prime;
      hash ^= ((t >> 16) & 0xFF); hash *= prime;
      hash ^= (t >> 24); hash *= prime;

      hash ^= (lvl & 0xFF); hash *= prime;
      hash ^= (lvl >> 8); hash *= prime;

      hash ^= (ln & 0xFF); hash *= prime;
      hash ^= (ln >> 8); hash *= prime;
  }

  return hash;
}
