#include "interface.h"

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

void store_u32(uint8_t* dst, uint32_t v) {
  dst[0] = static_cast<uint8_t>(v);
  dst[1] = static_cast<uint8_t>(v >> 8);
  dst[2] = static_cast<uint8_t>(v >> 16);
  dst[3] = static_cast<uint8_t>(v >> 24);
}

void store_u16(uint8_t* dst, uint16_t v) {
  dst[0] = static_cast<uint8_t>(v);
  dst[1] = static_cast<uint8_t>(v >> 8);
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  std::vector<uint8_t> out(ids.size() * 12u);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    for (std::size_t i = 0; i < ids.size(); ++i) {
      store_u32(ptr, ids[i]);
      ptr += 4;
      store_u32(ptr, ts[i]);
      ptr += 4;
      store_u16(ptr, levels[i]);
      ptr += 2;
      store_u16(ptr, lens[i]);
      ptr += 2;
    }
    hash = checksum_bytes(out);
  }
  return hash;
}
