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
  uint8_t* const base = out.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = base;
    
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t level = levels[i];
      uint16_t len = lens[i];
      
      std::memcpy(ptr, &id, 4);
      std::memcpy(ptr + 4, &t, 4);
      std::memcpy(ptr + 8, &level, 2);
      std::memcpy(ptr + 10, &len, 2);
      ptr += 12;
    }
    
    // FNV-1a checksum
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    hash = FNV_OFFSET;
    const uint8_t* data = base;
    std::size_t remaining = total_size;
    
    // Process 8 bytes at a time
    while (remaining >= 8) {
      hash ^= static_cast<uint64_t>(data[0]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[1]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[2]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[3]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[4]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[5]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[6]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[7]);
      hash *= FNV_PRIME;
      data += 8;
      remaining -= 8;
    }
    
    // Handle remaining bytes
    while (remaining > 0) {
      hash ^= static_cast<uint64_t>(*data);
      hash *= FNV_PRIME;
      ++data;
      --remaining;
    }
  }
  
  return hash;
}