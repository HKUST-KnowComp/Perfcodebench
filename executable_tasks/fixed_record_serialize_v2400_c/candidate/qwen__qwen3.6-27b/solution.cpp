#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  size_t n = ids.size();
  size_t buf_size = n * 12;
  std::vector<uint8_t> out(buf_size);
  uint8_t* ptr = out.data();

  for (size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    uint32_t t = ts[i];
    uint16_t lv = levels[i];
    uint16_t ln = lens[i];

    ptr[0] = static_cast<uint8_t>(id);
    ptr[1] = static_cast<uint8_t>(id >> 8);
    ptr[2] = static_cast<uint8_t>(id >> 16);
    ptr[3] = static_cast<uint8_t>(id >> 24);

    ptr[4] = static_cast<uint8_t>(t);
    ptr[5] = static_cast<uint8_t>(t >> 8);
    ptr[6] = static_cast<uint8_t>(t >> 16);
    ptr[7] = static_cast<uint8_t>(t >> 24);

    ptr[8] = static_cast<uint8_t>(lv);
    ptr[9] = static_cast<uint8_t>(lv >> 8);

    ptr[10] = static_cast<uint8_t>(ln);
    ptr[11] = static_cast<uint8_t>(ln >> 8);

    ptr += 12;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < buf_size; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }

  return hash;
}