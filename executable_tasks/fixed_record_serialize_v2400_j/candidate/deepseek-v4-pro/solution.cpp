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
  const uint32_t* id_ptr = ids.data();
  const uint32_t* ts_ptr = ts.data();
  const uint16_t* level_ptr = levels.data();
  const uint16_t* lens_ptr = lens.data();

  const uint64_t seed = 1469598103934665603ULL;
  const uint64_t multiplier = 1099511628211ULL;
  uint64_t result_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = seed;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = id_ptr[i];
      hash ^= (id & 0xFF); hash *= multiplier;
      hash ^= ((id >> 8) & 0xFF); hash *= multiplier;
      hash ^= ((id >> 16) & 0xFF); hash *= multiplier;
      hash ^= ((id >> 24) & 0xFF); hash *= multiplier;

      uint32_t t = ts_ptr[i];
      hash ^= (t & 0xFF); hash *= multiplier;
      hash ^= ((t >> 8) & 0xFF); hash *= multiplier;
      hash ^= ((t >> 16) & 0xFF); hash *= multiplier;
      hash ^= ((t >> 24) & 0xFF); hash *= multiplier;

      uint16_t level = level_ptr[i];
      hash ^= (level & 0xFF); hash *= multiplier;
      hash ^= ((level >> 8) & 0xFF); hash *= multiplier;

      uint16_t len = lens_ptr[i];
      hash ^= (len & 0xFF); hash *= multiplier;
      hash ^= ((len >> 8) & 0xFF); hash *= multiplier;
    }
    result_hash = hash;
  }
  return result_hash;
}