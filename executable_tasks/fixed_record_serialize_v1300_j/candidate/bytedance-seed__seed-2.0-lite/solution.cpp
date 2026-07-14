#include "interface.h"
#include <vector>

namespace {
const uint64_t FNV_INIT = 1469598103934665603ULL;
const uint64_t FNV_PRIME = 1099511628211ULL;
} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const size_t n = ids.size();
  const size_t total_bytes = n * 12; // 4+4+2+2 = 12 bytes per fixed record
  std::vector<uint8_t> out(total_bytes);
  uint8_t* write_ptr = out.data();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    write_ptr = out.data();
    uint64_t hash = FNV_INIT;

    for (size_t i = 0; i < n; ++i) {
      // Serialize and hash 32-bit id
      uint32_t id = ids[i];
      uint8_t b;
      b = static_cast<uint8_t>(id);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(id >> 8);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(id >> 16);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(id >> 24);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;

      // Serialize and hash 32-bit timestamp
      uint32_t t = ts[i];
      b = static_cast<uint8_t>(t);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(t >> 8);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(t >> 16);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(t >> 24);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;

      // Serialize and hash 16-bit level
      uint16_t lvl = levels[i];
      b = static_cast<uint8_t>(lvl);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(lvl >> 8);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;

      // Serialize and hash 16-bit length
      uint16_t len = lens[i];
      b = static_cast<uint8_t>(len);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
      b = static_cast<uint8_t>(len >> 8);
      *write_ptr++ = b;
      hash ^= b; hash *= FNV_PRIME;
    }
    final_hash = hash;
  }
  return final_hash;
}