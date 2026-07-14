#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t kFNVOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int /*iters*/) {
  uint64_t hash = kFNVOffsetBasis;
  std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v;
    v = ids[i];
    for (int j = 0; j < 4; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> (j * 8)));
      hash *= kFNVPrime;
    }
    v = ts[i];
    for (int j = 0; j < 4; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(v >> (j * 8)));
      hash *= kFNVPrime;
    }
    uint16_t u16 = levels[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(u16));
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(u16 >> 8));
    hash *= kFNVPrime;
    u16 = lens[i];
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(u16));
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(u16 >> 8));
    hash *= kFNVPrime;
  }
  return hash;
}
