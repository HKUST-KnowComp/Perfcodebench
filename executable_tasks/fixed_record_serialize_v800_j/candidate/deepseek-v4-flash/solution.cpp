#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    hash ^= static_cast<uint64_t>(id & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((id >> 8) & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((id >> 16) & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((id >> 24) & 0xff); hash *= fnv_prime;

    uint32_t t = ts[i];
    hash ^= static_cast<uint64_t>(t & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((t >> 8) & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((t >> 16) & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((t >> 24) & 0xff); hash *= fnv_prime;

    uint16_t lvl = levels[i];
    hash ^= static_cast<uint64_t>(lvl & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((lvl >> 8) & 0xff); hash *= fnv_prime;

    uint16_t len = lens[i];
    hash ^= static_cast<uint64_t>(len & 0xff); hash *= fnv_prime;
    hash ^= static_cast<uint64_t>((len >> 8) & 0xff); hash *= fnv_prime;
  }
  return hash;
}
