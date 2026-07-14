#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline void fnv1a(const uint8_t* __restrict p, std::size_t n, uint64_t& hash) {
  const uint64_t prime = 1099511628211ULL;
  for (const uint8_t* end = p + n; p != end; ++p) {
    hash ^= *p;
    hash *= prime;
  }
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t record_bytes = 4 + 4 + 2 + 2;
  const std::size_t total = n * record_bytes;

  std::vector<uint8_t> buffer;
  buffer.resize(total);

  uint8_t* __restrict out = buffer.data();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t id  = ids[i];
    const uint32_t t   = ts[i];
    const uint16_t lvl = levels[i];
    const uint16_t len = lens[i];

    out[0] = static_cast<uint8_t>(id);
    out[1] = static_cast<uint8_t>(id >> 8);
    out[2] = static_cast<uint8_t>(id >> 16);
    out[3] = static_cast<uint8_t>(id >> 24);

    out[4] = static_cast<uint8_t>(t);
    out[5] = static_cast<uint8_t>(t >> 8);
    out[6] = static_cast<uint8_t>(t >> 16);
    out[7] = static_cast<uint8_t>(t >> 24);

    out[8]  = static_cast<uint8_t>(lvl);
    out[9]  = static_cast<uint8_t>(lvl >> 8);

    out[10] = static_cast<uint8_t>(len);
    out[11] = static_cast<uint8_t>(len >> 8);

    out += record_bytes;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    fnv1a(buffer.data(), total, hash);
  }
  return hash;
}