#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + len;
  
  // Process 8 bytes at a time
  while (data + 8 <= end) {
    uint64_t chunk;
    std::memcpy(&chunk, data, 8);
    // Unroll the 8 byte operations
    hash ^= (chunk & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 8) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 16) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 24) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 32) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 40) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 48) & 0xFF);
    hash *= 1099511628211ULL;
    hash ^= ((chunk >> 56) & 0xFF);
    hash *= 1099511628211ULL;
    data += 8;
  }
  
  // Handle remaining bytes
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
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
  const size_t record_size = 12; // 4 + 4 + 2 + 2 bytes
  const size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (size_t i = 0; i < n; ++i) {
      // Write id (4 bytes, little-endian)
      uint32_t id = ids[i];
      std::memcpy(ptr, &id, 4);
      ptr += 4;
      
      // Write ts (4 bytes, little-endian)
      uint32_t t = ts[i];
      std::memcpy(ptr, &t, 4);
      ptr += 4;
      
      // Write level (2 bytes, little-endian)
      uint16_t level = levels[i];
      std::memcpy(ptr, &level, 2);
      ptr += 2;
      
      // Write len (2 bytes, little-endian)
      uint16_t len = lens[i];
      std::memcpy(ptr, &len, 2);
      ptr += 2;
    }
    
    hash = checksum_bytes(out.data(), total_size);
  }
  
  return hash;
}