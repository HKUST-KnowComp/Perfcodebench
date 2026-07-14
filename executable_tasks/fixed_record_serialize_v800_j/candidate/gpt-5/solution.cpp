#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;

  const std::size_t n = ids.size();
  const uint32_t* idp = ids.data();
  const uint32_t* tsp = ts.data();
  const uint16_t* lvlp = levels.data();
  const uint16_t* lenp = lens.data();

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32;
    uint16_t v16;

    // ids[i] as 4 bytes, little-endian
    v32 = idp[i];
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32));      h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 8)); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 16));h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 24));h *= FNV_PRIME;

    // ts[i] as 4 bytes, little-endian
    v32 = tsp[i];
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32));      h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 8)); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 16));h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v32 >> 24));h *= FNV_PRIME;

    // levels[i] as 2 bytes, little-endian
    v16 = lvlp[i];
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v16));      h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v16 >> 8)); h *= FNV_PRIME;

    // lens[i] as 2 bytes, little-endian
    v16 = lenp[i];
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v16));      h *= FNV_PRIME;
    h ^= static_cast<uint64_t>(static_cast<uint8_t>(v16 >> 8)); h *= FNV_PRIME;
  }

  return h;
}
