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

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  std::vector<uint8_t> out(ids.size() * 12); // Preallocate the output buffer
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    for (std::size_t i = 0; i < ids.size(); ++i) {
      // Use std::memcpy for efficient serialization
      std::memcpy(dst, &ids[i], sizeof(ids[i]));
      dst += sizeof(ids[i]);
      std::memcpy(dst, &ts[i], sizeof(ts[i]));
      dst += sizeof(ts[i]);
      std::memcpy(dst, &levels[i], sizeof(levels[i]));
      dst += sizeof(levels[i]);
      std::memcpy(dst, &lens[i], sizeof(lens[i]));
      dst += sizeof(lens[i]);
    }
    hash = checksum_bytes(out.data(), out.size());
  }
  return hash;
}