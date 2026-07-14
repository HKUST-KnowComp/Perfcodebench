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

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  size_t record_count = ids.size();
  size_t buffer_size = record_count * sizeof(uint32_t) * 2 + record_count * sizeof(uint16_t) * 2;
  std::vector<uint8_t> out(buffer_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    for (size_t i = 0; i < record_count; ++i) {
      *reinterpret_cast<uint32_t*>(dst) = ids[i];
      dst += sizeof(uint32_t);
      *reinterpret_cast<uint32_t*>(dst) = ts[i];
      dst += sizeof(uint32_t);
      *reinterpret_cast<uint16_t*>(dst) = levels[i];
      dst += sizeof(uint16_t);
      *reinterpret_cast<uint16_t*>(dst) = lens[i];
      dst += sizeof(uint16_t);
    }
    hash = checksum_bytes(out.data(), buffer_size);
  }
  return hash;
}