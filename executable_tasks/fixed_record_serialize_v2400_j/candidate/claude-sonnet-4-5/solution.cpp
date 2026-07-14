#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_inline(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  // Unroll by 4 for better ILP
  for (; i + 3 < len; i += 4) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i+1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i+2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i+3]);
    hash *= prime;
  }
  
  // Handle remainder
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
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
  const size_t record_size = 12; // 4 + 4 + 2 + 2
  const size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint8_t* ptr = out.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = ptr;
    
    for (size_t i = 0; i < n; ++i) {
      // Write id (u32, little-endian)
      uint32_t id = ids[i];
      std::memcpy(p, &id, 4);
      p += 4;
      
      // Write ts (u32, little-endian)
      uint32_t t = ts[i];
      std::memcpy(p, &t, 4);
      p += 4;
      
      // Write level (u16, little-endian)
      uint16_t level = levels[i];
      std::memcpy(p, &level, 2);
      p += 2;
      
      // Write len (u16, little-endian)
      uint16_t len = lens[i];
      std::memcpy(p, &len, 2);
      p += 2;
    }
    
    hash = checksum_inline(ptr, total_size);
  }
  
  return hash;
}
