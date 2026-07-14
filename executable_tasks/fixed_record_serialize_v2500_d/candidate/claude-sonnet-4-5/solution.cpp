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
    hash = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t level = levels[i];
      uint16_t len = lens[i];
      
      // Write id (4 bytes)
      std::memcpy(ptr, &id, 4);
      hash ^= ptr[0]; hash *= 1099511628211ULL;
      hash ^= ptr[1]; hash *= 1099511628211ULL;
      hash ^= ptr[2]; hash *= 1099511628211ULL;
      hash ^= ptr[3]; hash *= 1099511628211ULL;
      ptr += 4;
      
      // Write ts (4 bytes)
      std::memcpy(ptr, &t, 4);
      hash ^= ptr[0]; hash *= 1099511628211ULL;
      hash ^= ptr[1]; hash *= 1099511628211ULL;
      hash ^= ptr[2]; hash *= 1099511628211ULL;
      hash ^= ptr[3]; hash *= 1099511628211ULL;
      ptr += 4;
      
      // Write level (2 bytes)
      std::memcpy(ptr, &level, 2);
      hash ^= ptr[0]; hash *= 1099511628211ULL;
      hash ^= ptr[1]; hash *= 1099511628211ULL;
      ptr += 2;
      
      // Write len (2 bytes)
      std::memcpy(ptr, &len, 2);
      hash ^= ptr[0]; hash *= 1099511628211ULL;
      hash ^= ptr[1]; hash *= 1099511628211ULL;
      ptr += 2;
    }
  }
  
  return hash;
}