#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes_inline(const uint8_t* bytes, size_t len) {
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
  const size_t n = ids.size();
  const size_t record_size = 12;  // 4 + 4 + 2 + 2
  const size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (size_t i = 0; i < n; ++i) {
      // Write id (uint32_t, little-endian)
      uint32_t id = ids[i];
      ptr[0] = static_cast<uint8_t>(id);
      ptr[1] = static_cast<uint8_t>(id >> 8);
      ptr[2] = static_cast<uint8_t>(id >> 16);
      ptr[3] = static_cast<uint8_t>(id >> 24);
      
      // Write ts (uint32_t, little-endian)
      uint32_t t = ts[i];
      ptr[4] = static_cast<uint8_t>(t);
      ptr[5] = static_cast<uint8_t>(t >> 8);
      ptr[6] = static_cast<uint8_t>(t >> 16);
      ptr[7] = static_cast<uint8_t>(t >> 24);
      
      // Write level (uint16_t, little-endian)
      uint16_t level = levels[i];
      ptr[8] = static_cast<uint8_t>(level);
      ptr[9] = static_cast<uint8_t>(level >> 8);
      
      // Write len (uint16_t, little-endian)
      uint16_t len = lens[i];
      ptr[10] = static_cast<uint8_t>(len);
      ptr[11] = static_cast<uint8_t>(len >> 8);
      
      ptr += record_size;
    }
    
    hash = checksum_bytes_inline(out.data(), total_size);
  }
  
  return hash;
}