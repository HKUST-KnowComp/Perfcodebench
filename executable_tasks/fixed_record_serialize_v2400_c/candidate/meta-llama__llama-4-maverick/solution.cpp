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
  const size_t record_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
  const size_t total_size = ids.size() * record_size;
  std::vector<uint8_t> out(total_size);
  uint8_t* out_ptr = out.data();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    std::memcpy(out_ptr, &ids[i], sizeof(uint32_t));
    out_ptr += sizeof(uint32_t);
    std::memcpy(out_ptr, &ts[i], sizeof(uint32_t));
    out_ptr += sizeof(uint32_t);
    std::memcpy(out_ptr, &levels[i], sizeof(uint16_t));
    out_ptr += sizeof(uint16_t);
    std::memcpy(out_ptr, &lens[i], sizeof(uint16_t));
    out_ptr += sizeof(uint16_t);
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}
