#include "interface.h"

#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    hash ^= id & 0xFFULL;       hash *= 1099511628211ULL;
    hash ^= (id >> 8) & 0xFFULL; hash *= 1099511628211ULL;
    hash ^= (id >> 16) & 0xFFULL;hash *= 1099511628211ULL;
    hash ^= (id >> 24);         hash *= 1099511628211ULL;
    uint32_t t = ts[i];
    hash ^= t & 0xFFULL;        hash *= 1099511628211ULL;
    hash ^= (t >> 8) & 0xFFULL; hash *= 1099511628211ULL;
    hash ^= (t >> 16) & 0xFFULL;hash *= 1099511628211ULL;
    hash ^= (t >> 24);          hash *= 1099511628211ULL;
    uint16_t lv = levels[i];
    hash ^= lv & 0xFFULL;       hash *= 1099511628211ULL;
    hash ^= (lv >> 8);          hash *= 1099511628211ULL;
    uint16_t ln = lens[i];
    hash ^= ln & 0xFFULL;       hash *= 1099511628211ULL;
    hash ^= (ln >> 8);          hash *= 1099511628211ULL;
  }
  return hash;
}