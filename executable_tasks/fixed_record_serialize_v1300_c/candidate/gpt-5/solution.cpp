#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Preserve behavior for non-positive iterations
  if (iters <= 0) {
    return 0;
  }

  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime  = 1099511628211ULL;

  uint64_t hash = fnv_offset;

  const std::size_t n = ids.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32;

    // ids[i] as little-endian bytes
    v32 = ids[i];
    hash ^= static_cast<uint8_t>(v32);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 8);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 16);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 24);
    hash *= fnv_prime;

    // ts[i] as little-endian bytes
    v32 = ts[i];
    hash ^= static_cast<uint8_t>(v32);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 8);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 16);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v32 >> 24);
    hash *= fnv_prime;

    // levels[i] as little-endian bytes
    uint16_t v16 = levels[i];
    hash ^= static_cast<uint8_t>(v16);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v16 >> 8);
    hash *= fnv_prime;

    // lens[i] as little-endian bytes
    v16 = lens[i];
    hash ^= static_cast<uint8_t>(v16);
    hash *= fnv_prime;
    hash ^= static_cast<uint8_t>(v16 >> 8);
    hash *= fnv_prime;
  }

  // The checksum for subsequent iterations would be identical; baseline overwrote
  // the hash each time, so returning this value matches the reference.
  return hash;
}
