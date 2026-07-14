#include "interface.h"

#include <cstdint>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// No helper functions; all logic inlined.

}  // anonymous namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

  uint64_t final_hash = 0;
  const std::size_t n = ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET;

    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      hash ^= static_cast<uint8_t>(id);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 8);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 16);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(id >> 24);
      hash *= FNV_PRIME;

      uint32_t t = ts[i];
      hash ^= static_cast<uint8_t>(t);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 8);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 16);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(t >> 24);
      hash *= FNV_PRIME;

      uint16_t level = levels[i];
      hash ^= static_cast<uint8_t>(level);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(level >> 8);
      hash *= FNV_PRIME;

      uint16_t len = lens[i];
      hash ^= static_cast<uint8_t>(len);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint8_t>(len >> 8);
      hash *= FNV_PRIME;
    }

    final_hash = hash;
  }

  return final_hash;
}
