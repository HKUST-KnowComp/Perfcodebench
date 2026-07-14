#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = ids.size();
  const std::size_t total_bytes = n * 12;
  std::vector<uint8_t> buf(total_bytes);

  uint8_t* p = buf.data();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t id = ids[i];
    p[0] = static_cast<uint8_t>(id);
    p[1] = static_cast<uint8_t>(id >> 8);
    p[2] = static_cast<uint8_t>(id >> 16);
    p[3] = static_cast<uint8_t>(id >> 24);

    const uint32_t ts_val = ts[i];
    p[4] = static_cast<uint8_t>(ts_val);
    p[5] = static_cast<uint8_t>(ts_val >> 8);
    p[6] = static_cast<uint8_t>(ts_val >> 16);
    p[7] = static_cast<uint8_t>(ts_val >> 24);

    const uint16_t lv = levels[i];
    p[8] = static_cast<uint8_t>(lv);
    p[9] = static_cast<uint8_t>(lv >> 8);

    const uint16_t lnv = lens[i];
    p[10] = static_cast<uint8_t>(lnv);
    p[11] = static_cast<uint8_t>(lnv >> 8);

    p += 12;
  }

  return checksum_bytes(buf);
}