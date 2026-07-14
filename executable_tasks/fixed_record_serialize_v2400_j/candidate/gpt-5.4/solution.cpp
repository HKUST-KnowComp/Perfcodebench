#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_bytes_ptr(const uint8_t* data, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t bytes_per_record = 12;
  const std::size_t total_bytes = n * bytes_per_record;

  std::vector<uint8_t> out(total_bytes);
  uint64_t hash = 0;

  const uint32_t* ids_p = ids.data();
  const uint32_t* ts_p = ts.data();
  const uint16_t* levels_p = levels.data();
  const uint16_t* lens_p = lens.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out.data();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t id = ids_p[i];
      const uint32_t t = ts_p[i];
      const uint16_t lvl = levels_p[i];
      const uint16_t len = lens_p[i];

      p[0] = static_cast<uint8_t>(id);
      p[1] = static_cast<uint8_t>(id >> 8);
      p[2] = static_cast<uint8_t>(id >> 16);
      p[3] = static_cast<uint8_t>(id >> 24);

      p[4] = static_cast<uint8_t>(t);
      p[5] = static_cast<uint8_t>(t >> 8);
      p[6] = static_cast<uint8_t>(t >> 16);
      p[7] = static_cast<uint8_t>(t >> 24);

      p[8] = static_cast<uint8_t>(lvl);
      p[9] = static_cast<uint8_t>(lvl >> 8);

      p[10] = static_cast<uint8_t>(len);
      p[11] = static_cast<uint8_t>(len >> 8);

      p += 12;
    }
    hash = checksum_bytes_ptr(out.data(), total_bytes);
  }

  return hash;
}
