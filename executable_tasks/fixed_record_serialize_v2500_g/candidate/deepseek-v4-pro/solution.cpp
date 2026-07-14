#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0ULL;

  const std::size_t N = ids.size();
  const uint64_t prime = 1099511628211ULL;
  uint64_t hash = 1469598103934665603ULL;

  for (std::size_t i = 0; i < N; ++i) {
    uint32_t id = ids[i];
    uint32_t t  = ts[i];
    uint16_t lv = levels[i];
    uint16_t ln = lens[i];

    // id bytes (little-endian)
    hash ^= static_cast<uint8_t>(id);       hash *= prime;
    hash ^= static_cast<uint8_t>(id >> 8);  hash *= prime;
    hash ^= static_cast<uint8_t>(id >> 16); hash *= prime;
    hash ^= static_cast<uint8_t>(id >> 24); hash *= prime;

    // ts bytes
    hash ^= static_cast<uint8_t>(t);        hash *= prime;
    hash ^= static_cast<uint8_t>(t >> 8);   hash *= prime;
    hash ^= static_cast<uint8_t>(t >> 16);  hash *= prime;
    hash ^= static_cast<uint8_t>(t >> 24);  hash *= prime;

    // levels bytes
    hash ^= static_cast<uint8_t>(lv);       hash *= prime;
    hash ^= static_cast<uint8_t>(lv >> 8);  hash *= prime;

    // lens bytes
    hash ^= static_cast<uint8_t>(ln);       hash *= prime;
    hash ^= static_cast<uint8_t>(ln >> 8);  hash *= prime;
  }

  return hash;
}
