#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const size_t n = ids.size();
  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_levels = levels.data();
  const uint16_t* p_lens = lens.data();

  uint64_t hash = 0;
  constexpr uint64_t fnv_prime = 1099511628211ULL;
  constexpr uint64_t fnv_offset = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (size_t i = 0; i < n; ++i) {
      uint32_t id = p_ids[i];
      uint32_t t = p_ts[i];
      uint16_t l = p_levels[i];
      uint16_t ln = p_lens[i];

      hash ^= static_cast<uint8_t>(id); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(id >> 8); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(id >> 16); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(id >> 24); hash *= fnv_prime;

      hash ^= static_cast<uint8_t>(t); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(t >> 8); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(t >> 16); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(t >> 24); hash *= fnv_prime;

      hash ^= static_cast<uint8_t>(l); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(l >> 8); hash *= fnv_prime;

      hash ^= static_cast<uint8_t>(ln); hash *= fnv_prime;
      hash ^= static_cast<uint8_t>(ln >> 8); hash *= fnv_prime;
    }
  }
  return hash;
}
