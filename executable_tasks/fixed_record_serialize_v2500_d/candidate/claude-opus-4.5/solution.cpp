#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  // Process 8 bytes at a time
  size_t i = 0;
  size_t len8 = len & ~size_t(7);
  for (; i < len8; i += 8) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 7]);
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
  uint8_t* ptr = out.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    ptr = out.data();
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t level = levels[i];
      uint16_t len = lens[i];
      
      std::memcpy(ptr, &id, 4);
      ptr += 4;
      std::memcpy(ptr, &t, 4);
      ptr += 4;
      std::memcpy(ptr, &level, 2);
      ptr += 2;
      std::memcpy(ptr, &len, 2);
      ptr += 2;
    }
    
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}