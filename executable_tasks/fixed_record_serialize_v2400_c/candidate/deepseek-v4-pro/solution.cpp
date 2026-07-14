#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t kHashInitial = 1469598103934665603ULL;
constexpr uint64_t kPrime = 1099511628211ULL;

void hash_byte(uint64_t& hash, uint8_t b) {
    hash ^= static_cast<uint64_t>(b);
    hash *= kPrime;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t hash = kHashInitial;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    hash_byte(hash, static_cast<uint8_t>(id));
    hash_byte(hash, static_cast<uint8_t>(id >> 8));
    hash_byte(hash, static_cast<uint8_t>(id >> 16));
    hash_byte(hash, static_cast<uint8_t>(id >> 24));

    uint32_t t = ts[i];
    hash_byte(hash, static_cast<uint8_t>(t));
    hash_byte(hash, static_cast<uint8_t>(t >> 8));
    hash_byte(hash, static_cast<uint8_t>(t >> 16));
    hash_byte(hash, static_cast<uint8_t>(t >> 24));

    uint16_t lvl = levels[i];
    hash_byte(hash, static_cast<uint8_t>(lvl));
    hash_byte(hash, static_cast<uint8_t>(lvl >> 8));

    uint16_t len = lens[i];
    hash_byte(hash, static_cast<uint8_t>(len));
    hash_byte(hash, static_cast<uint8_t>(len >> 8));
  }
  return hash;
}