#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void serialize_records(uint8_t* out, const std::vector<uint32_t>& ids, const std::vector<uint32_t>& ts, const std::vector<uint16_t>& levels, const std::vector<uint16_t>& lens) {
  size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    uint32_t t = ts[i];
    uint16_t level = levels[i];
    uint16_t len = lens[i];
    out[0] = static_cast<uint8_t>(id);
    out[1] = static_cast<uint8_t>(id >> 8);
    out[2] = static_cast<uint8_t>(id >> 16);
    out[3] = static_cast<uint8_t>(id >> 24);
    out[4] = static_cast<uint8_t>(t);
    out[5] = static_cast<uint8_t>(t >> 8);
    out[6] = static_cast<uint8_t>(t >> 16);
    out[7] = static_cast<uint8_t>(t >> 24);
    out[8] = static_cast<uint8_t>(level);
    out[9] = static_cast<uint8_t>(level >> 8);
    out[10] = static_cast<uint8_t>(len);
    out[11] = static_cast<uint8_t>(len >> 8);
    out += 12;
  }
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  size_t n = ids.size();
  size_t buf_size = n * 12;
  std::vector<uint8_t> buf(buf_size);
  uint8_t* out = buf.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    serialize_records(out, ids, ts, levels, lens);
    hash = checksum_bytes(out, buf_size);
  }
  return hash;
}