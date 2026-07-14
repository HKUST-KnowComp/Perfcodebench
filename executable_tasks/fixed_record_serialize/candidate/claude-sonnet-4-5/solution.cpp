#include "interface.h"

#include <cstring>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t bytes_per_record = 12; // 4 + 4 + 2 + 2
  const std::size_t total_bytes = n * bytes_per_record;
  
  std::vector<uint8_t> out(total_bytes);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    // Serialize all records directly into buffer
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t level = levels[i];
      uint16_t len = lens[i];
      
      // Write id (4 bytes)
      std::memcpy(ptr, &id, 4);
      ptr += 4;
      
      // Write ts (4 bytes)
      std::memcpy(ptr, &t, 4);
      ptr += 4;
      
      // Write level (2 bytes)
      std::memcpy(ptr, &level, 2);
      ptr += 2;
      
      // Write len (2 bytes)
      std::memcpy(ptr, &len, 2);
      ptr += 2;
    }
    
    // Compute checksum inline
    hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const uint8_t* data = out.data();
    std::size_t i = 0;
    
    // Unroll by 4 for better performance
    const std::size_t unroll_limit = total_bytes - (total_bytes % 4);
    for (; i < unroll_limit; i += 4) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= prime;
      hash ^= static_cast<uint64_t>(data[i + 1]);
      hash *= prime;
      hash ^= static_cast<uint64_t>(data[i + 2]);
      hash *= prime;
      hash ^= static_cast<uint64_t>(data[i + 3]);
      hash *= prime;
    }
    
    // Handle remaining bytes
    for (; i < total_bytes; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= prime;
    }
  }
  
  return hash;
}