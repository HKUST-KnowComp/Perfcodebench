#include "interface.h"
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t num_records = ids.size();
  const std::size_t buffer_size = num_records * 12; // 4+4+2+2 = 12 bytes per record
  std::vector<uint8_t> out(buffer_size);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    uint64_t hash = FNV_OFFSET;

    for (std::size_t i = 0; i < num_records; ++i) {
      // Write and hash 32-bit ID (matches original little-endian order)
      uint32_t id = ids[i];
      for (int b = 0; b < 4; ++b) {
        const uint8_t byte = static_cast<uint8_t>(id >> (8 * b));
        *dst++ = byte;
        hash ^= byte;
        hash *= FNV_PRIME;
      }
      // Write and hash 32-bit timestamp
      uint32_t t = ts[i];
      for (int b = 0; b < 4; ++b) {
        const uint8_t byte = static_cast<uint8_t>(t >> (8 * b));
        *dst++ = byte;
        hash ^= byte;
        hash *= FNV_PRIME;
      }
      // Write and hash 16-bit level
      uint16_t lvl = levels[i];
      for (int b = 0; b < 2; ++b) {
        const uint8_t byte = static_cast<uint8_t>(lvl >> (8 * b));
        *dst++ = byte;
        hash ^= byte;
        hash *= FNV_PRIME;
      }
      // Write and hash 16-bit length
      uint16_t len = lens[i];
      for (int b = 0; b < 2; ++b) {
        const uint8_t byte = static_cast<uint8_t>(len >> (8 * b));
        *dst++ = byte;
        hash ^= byte;
        hash *= FNV_PRIME;
      }
    }
    final_hash = hash;
  }
  return final_hash;
}