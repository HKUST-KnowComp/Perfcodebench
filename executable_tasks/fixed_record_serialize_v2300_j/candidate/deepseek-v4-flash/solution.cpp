#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t fnv1a(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
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
  // All vectors are assumed to have the same length; handle empty gracefully.
  size_t n = ids.size();
  if (n == 0 || iters <= 0) {
    return 0;
  }

  // Preallocate exact buffer for one full serialization (12 bytes per record).
  std::vector<uint8_t> out(n * 12);
  uint8_t* ptr = out.data();

  for (size_t i = 0; i < n; ++i) {
    uint32_t id = ids[i];
    *ptr++ = static_cast<uint8_t>(id);
    *ptr++ = static_cast<uint8_t>(id >> 8);
    *ptr++ = static_cast<uint8_t>(id >> 16);
    *ptr++ = static_cast<uint8_t>(id >> 24);

    uint32_t t = ts[i];
    *ptr++ = static_cast<uint8_t>(t);
    *ptr++ = static_cast<uint8_t>(t >> 8);
    *ptr++ = static_cast<uint8_t>(t >> 16);
    *ptr++ = static_cast<uint8_t>(t >> 24);

    uint16_t level = levels[i];
    *ptr++ = static_cast<uint8_t>(level);
    *ptr++ = static_cast<uint8_t>(level >> 8);

    uint16_t len = lens[i];
    *ptr++ = static_cast<uint8_t>(len);
    *ptr++ = static_cast<uint8_t>(len >> 8);
  }

  // Compute FNV-1a checksum of the serialized buffer.
  return fnv1a(out.data(), out.size());
}
