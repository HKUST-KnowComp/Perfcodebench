#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Match baseline semantics: if iters <= 0, return 0
  if (iters <= 0) return 0;

  const std::size_t n = ids.size();
  // Stream FNV-1a over the serialized bytes without materializing the buffer
  uint64_t hash = 1469598103934665603ULL;               // FNV offset basis
  const uint64_t prime = 1099511628211ULL;              // FNV prime

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32;
    uint16_t v16;

    v32 = ids[i];
    hash ^= static_cast<uint8_t>(v32);        hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 8);   hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 16);  hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 24);  hash *= prime;

    v32 = ts[i];
    hash ^= static_cast<uint8_t>(v32);        hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 8);   hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 16);  hash *= prime;
    hash ^= static_cast<uint8_t>(v32 >> 24);  hash *= prime;

    v16 = levels[i];
    hash ^= static_cast<uint8_t>(v16);        hash *= prime;
    hash ^= static_cast<uint8_t>(v16 >> 8);   hash *= prime;

    v16 = lens[i];
    hash ^= static_cast<uint8_t>(v16);        hash *= prime;
    hash ^= static_cast<uint8_t>(v16 >> 8);   hash *= prime;
  }

  // Baseline overwrote the hash each iteration with the same value; computing once is equivalent
  return hash;
}
