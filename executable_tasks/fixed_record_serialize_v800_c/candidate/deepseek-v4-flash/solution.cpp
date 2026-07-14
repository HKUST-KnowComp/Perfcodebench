#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t fnv1a_byte(uint64_t h, uint8_t b) {
    h ^= b;
    h *= 1099511628211ULL;
    return h;
}

inline uint64_t hash_u32(uint64_t h, uint32_t v) {
    h = fnv1a_byte(h, static_cast<uint8_t>(v));
    h = fnv1a_byte(h, static_cast<uint8_t>(v >> 8));
    h = fnv1a_byte(h, static_cast<uint8_t>(v >> 16));
    h = fnv1a_byte(h, static_cast<uint8_t>(v >> 24));
    return h;
}

inline uint64_t hash_u16(uint64_t h, uint16_t v) {
    h = fnv1a_byte(h, static_cast<uint8_t>(v));
    h = fnv1a_byte(h, static_cast<uint8_t>(v >> 8));
    return h;
}

} // unnamed namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  uint64_t last_hash = 0;
  const std::size_t n = ids.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      h = hash_u32(h, ids[i]);
      h = hash_u32(h, ts[i]);
      h = hash_u16(h, levels[i]);
      h = hash_u16(h, lens[i]);
    }
    last_hash = h;
  }
  return last_hash;
}
