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
  const std::size_t total_bytes = n * record_size;
  
  std::vector<uint8_t> out(total_bytes);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      std::memcpy(ptr, &ids[i], 4);
      std::memcpy(ptr + 4, &ts[i], 4);
      std::memcpy(ptr + 8, &levels[i], 2);
      std::memcpy(ptr + 10, &lens[i], 2);
      ptr += 12;
    }
    
    // FNV-1a hash
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    hash = FNV_OFFSET;
    
    const uint8_t* data = out.data();
    std::size_t len = total_bytes;
    std::size_t i = 0;
    
    // Process 8 bytes at a time for better throughput
    for (; i + 8 <= len; i += 8) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 1]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 2]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 3]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 4]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 5]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 6]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 7]);
      hash *= FNV_PRIME;
    }
    
    // Handle remaining bytes
    for (; i < len; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= FNV_PRIME;
    }
  }
  
  return hash;
}