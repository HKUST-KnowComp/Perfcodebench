#include "interface.h"

#include <vector>
#include <cstring> // for std::memcpy

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void serialize_records(uint8_t* out, const std::vector<uint32_t>& ids, const std::vector<uint32_t>& ts, const std::vector<uint16_t>& levels, const std::vector<uint16_t>& lens) {
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    std::memcpy(out, &ids[i], sizeof(uint32_t));
    out += sizeof(uint32_t);
    std::memcpy(out, &ts[i], sizeof(uint32_t));
    out += sizeof(uint32_t);
    std::memcpy(out, &levels[i], sizeof(uint16_t));
    out += sizeof(uint16_t);
    std::memcpy(out, &lens[i], sizeof(uint16_t));
    out += sizeof(uint16_t);
  }
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t record_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
  const std::size_t total_size = ids.size() * record_size;
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    serialize_records(out.data(), ids, ts, levels, lens);
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}