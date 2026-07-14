#include "interface.h"

#include <vector>
#include <cstring> // for memcpy

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void serialize_records(std::vector<uint8_t>& out, const std::vector<uint32_t>& ids, const std::vector<uint32_t>& ts, const std::vector<uint16_t>& levels, const std::vector<uint16_t>& lens) {
  uint8_t* dst = out.data();
  for (size_t i = 0; i < ids.size(); ++i) {
    *reinterpret_cast<uint32_t*>(dst) = ids[i];
    dst += sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(dst) = ts[i];
    dst += sizeof(uint32_t);
    *reinterpret_cast<uint16_t*>(dst) = levels[i];
    dst += sizeof(uint16_t);
    *reinterpret_cast<uint16_t*>(dst) = lens[i];
    dst += sizeof(uint16_t);
  }
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const size_t record_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
  const size_t total_size = ids.size() * record_size;
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    serialize_records(out, ids, ts, levels, lens);
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}