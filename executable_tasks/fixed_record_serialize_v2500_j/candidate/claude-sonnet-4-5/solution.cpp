#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes_fast(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  // Process 8 bytes at a time
  for (; i + 7 < len; i += 8) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i+7]);
    hash *= prime;
  }
  
  // Handle remaining bytes
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (size_t i = 0; i < n; ++i) {
      // Write id (4 bytes, little-endian)
      std::memcpy(ptr, &ids[i], 4);
      ptr += 4;
      
      // Write ts (4 bytes, little-endian)
      std::memcpy(ptr, &ts[i], 4);
      ptr += 4;
      
      // Write level (2 bytes, little-endian)
      std::memcpy(ptr, &levels[i], 2);
      ptr += 2;
      
      // Write len (2 bytes, little-endian)
      std::memcpy(ptr, &lens[i], 2);
      ptr += 2;
    }
    
    hash = checksum_bytes_fast(out.data(), total_size);
  }
  
  return hash;
}