#include "interface.h"

#include <vector>
#include <cstring> // for memcpy

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void serialize_record(uint8_t* out, uint32_t id, uint32_t ts, uint16_t level, uint16_t len) {
  std::memcpy(out, &id, sizeof(id));
  out += sizeof(id);
  std::memcpy(out, &ts, sizeof(ts));
  out += sizeof(ts);
  std::memcpy(out, &level, sizeof(level));
  out += sizeof(level);
  std::memcpy(out, &len, sizeof(len));
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const size_t record_size = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2;
  const size_t total_size = ids.size() * record_size;
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* out_ptr = out.data();
    for (size_t i = 0; i < ids.size(); ++i) {
      serialize_record(out_ptr, ids[i], ts[i], levels[i], lens[i]);
      out_ptr += record_size;
    }
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}