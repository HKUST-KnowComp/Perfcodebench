#include "interface.h"

#include <cstdint>
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
    
    // Serialize all records
    for (std::size_t i = 0; i < n; ++i) {
      std::memcpy(ptr, &ids[i], 4);
      std::memcpy(ptr + 4, &ts[i], 4);
      std::memcpy(ptr + 8, &levels[i], 2);
      std::memcpy(ptr + 10, &lens[i], 2);
      ptr += 12;
    }
    
    // FNV-1a checksum - process 8 bytes at a time
    hash = 1469598103934665603ULL;
    const uint8_t* data = out.data();
    std::size_t remaining = total_bytes;
    
    // Process 8 bytes at a time
    while (remaining >= 8) {
      uint64_t chunk;
      std::memcpy(&chunk, data, 8);
      // Process each byte of the chunk
      hash ^= chunk & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 8) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 16) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 24) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 32) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 40) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 48) & 0xFF;
      hash *= 1099511628211ULL;
      hash ^= (chunk >> 56) & 0xFF;
      hash *= 1099511628211ULL;
      data += 8;
      remaining -= 8;
    }
    
    // Handle remaining bytes
    while (remaining > 0) {
      hash ^= static_cast<uint64_t>(*data);
      hash *= 1099511628211ULL;
      ++data;
      --remaining;
    }
  }
  
  return hash;
}