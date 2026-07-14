#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    // Serialize ids[i] (u32, little-endian)
    uint32_t id = ids[i];
    hash ^= static_cast<uint64_t>(id);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(id >> 8);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(id >> 16);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(id >> 24);
    hash *= fnv_prime;

    // Serialize ts[i] (u32, little-endian)
    uint32_t t = ts[i];
    hash ^= static_cast<uint64_t>(t);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(t >> 8);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(t >> 16);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(t >> 24);
    hash *= fnv_prime;

    // Serialize levels[i] (u16, little-endian)
    uint16_t l = levels[i];
    hash ^= static_cast<uint64_t>(l);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(l >> 8);
    hash *= fnv_prime;

    // Serialize lens[i] (u16, little-endian)
    uint16_t len = lens[i];
    hash ^= static_cast<uint64_t>(len);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(len >> 8);
    hash *= fnv_prime;
  }

  return hash;
}