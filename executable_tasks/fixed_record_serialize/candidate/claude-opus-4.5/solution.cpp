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
  const std::size_t record_size = 12; // 4 + 4 + 2 + 2
  const std::size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint8_t* const buf = out.data();
  
  uint64_t hash = 0;
  
  const uint32_t* ids_ptr = ids.data();
  const uint32_t* ts_ptr = ts.data();
  const uint16_t* levels_ptr = levels.data();
  const uint16_t* lens_ptr = lens.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Serialize all records
    uint8_t* p = buf;
    for (std::size_t i = 0; i < n; ++i) {
      std::memcpy(p, &ids_ptr[i], 4);
      std::memcpy(p + 4, &ts_ptr[i], 4);
      std::memcpy(p + 8, &levels_ptr[i], 2);
      std::memcpy(p + 10, &lens_ptr[i], 2);
      p += 12;
    }
    
    // Compute FNV-1a hash
    hash = 1469598103934665603ULL;
    const uint8_t* hp = buf;
    const uint8_t* end = buf + total_size;
    
    // Process 8 bytes at a time
    while (hp + 8 <= end) {
      hash ^= static_cast<uint64_t>(hp[0]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[1]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[2]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[3]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[4]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[5]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[6]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(hp[7]);
      hash *= 1099511628211ULL;
      hp += 8;
    }
    
    // Handle remaining bytes
    while (hp < end) {
      hash ^= static_cast<uint64_t>(*hp++);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}