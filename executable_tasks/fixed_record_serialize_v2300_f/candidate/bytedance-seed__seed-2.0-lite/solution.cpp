#include "interface.h"

#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t N = ids.size();
  const std::size_t total_bytes = N * 12;
  std::vector<uint8_t> out;
  if (total_bytes > 0) {
    out.resize(total_bytes);
  }
  
  uint64_t final_hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    uint64_t hash = FNV_OFFSET;

    for (std::size_t i = 0; i < N; ++i) {
      // Write and hash 32-bit ID (little-endian, matches original byte order)
      uint32_t id = ids[i];
      *ptr++ = static_cast<uint8_t>(id);
      *ptr++ = static_cast<uint8_t>(id >> 8);
      *ptr++ = static_cast<uint8_t>(id >> 16);
      *ptr++ = static_cast<uint8_t>(id >> 24);
      hash ^= static_cast<uint64_t>(ptr[-4]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-3]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-2]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-1]); hash *= FNV_PRIME;

      // Write and hash 32-bit timestamp
      uint32_t t = ts[i];
      *ptr++ = static_cast<uint8_t>(t);
      *ptr++ = static_cast<uint8_t>(t >> 8);
      *ptr++ = static_cast<uint8_t>(t >> 16);
      *ptr++ = static_cast<uint8_t>(t >> 24);
      hash ^= static_cast<uint64_t>(ptr[-4]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-3]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-2]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-1]); hash *= FNV_PRIME;

      // Write and hash 16-bit level
      uint16_t lvl = levels[i];
      *ptr++ = static_cast<uint8_t>(lvl);
      *ptr++ = static_cast<uint8_t>(lvl >> 8);
      hash ^= static_cast<uint64_t>(ptr[-2]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-1]); hash *= FNV_PRIME;

      // Write and hash 16-bit length
      uint16_t len = lens[i];
      *ptr++ = static_cast<uint8_t>(len);
      *ptr++ = static_cast<uint8_t>(len >> 8);
      hash ^= static_cast<uint64_t>(ptr[-2]); hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(ptr[-1]); hash *= FNV_PRIME;
    }
    final_hash = hash;
  }
  return final_hash;
}