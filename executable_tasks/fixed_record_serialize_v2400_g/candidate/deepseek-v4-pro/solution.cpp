#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int /*iters*/) {
  const size_t N = ids.size();
  const uint32_t* id_ptr = ids.data();
  const uint32_t* ts_ptr = ts.data();
  const uint16_t* lvl_ptr = levels.data();
  const uint16_t* len_ptr = lens.data();

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < N; ++i) {
    uint32_t id = id_ptr[i];
    for (int j = 0; j < 4; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(id));
      hash *= 1099511628211ULL;
      id >>= 8;
    }
    uint32_t t = ts_ptr[i];
    for (int j = 0; j < 4; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(t));
      hash *= 1099511628211ULL;
      t >>= 8;
    }
    uint16_t lvl = lvl_ptr[i];
    for (int j = 0; j < 2; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(lvl));
      hash *= 1099511628211ULL;
      lvl >>= 8;
    }
    uint16_t len = len_ptr[i];
    for (int j = 0; j < 2; ++j) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len));
      hash *= 1099511628211ULL;
      len >>= 8;
    }
  }
  return hash;
}
