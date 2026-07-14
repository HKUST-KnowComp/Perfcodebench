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
      ptr += record_size;
    }
    
    // FNV-1a checksum
    hash = 1469598103934665603ULL;
    const uint8_t* data = out.data();
    std::size_t len = total_bytes;
    
    // Process 8 bytes at a time
    std::size_t chunks = len / 8;
    for (std::size_t i = 0; i < chunks; ++i) {
      hash ^= static_cast<uint64_t>(data[0]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[1]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[2]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[3]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[4]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[5]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[6]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[7]);
      hash *= 1099511628211ULL;
      data += 8;
    }
    
    // Handle remaining bytes
    std::size_t remaining = len % 8;
    for (std::size_t i = 0; i < remaining; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}