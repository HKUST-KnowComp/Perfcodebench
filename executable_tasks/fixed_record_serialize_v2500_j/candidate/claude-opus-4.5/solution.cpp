#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  
  // Process 8 bytes at a time
  for (; i + 8 <= len; i += 8) {
    __builtin_prefetch(data + i + 64, 0, 0);
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
  constexpr size_t record_size = 12; // 4 + 4 + 2 + 2
  const size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint8_t* ptr = out.data();
  
  // Serialize once
  for (size_t i = 0; i < n; ++i) {
    std::memcpy(ptr, &ids[i], 4);
    ptr += 4;
    std::memcpy(ptr, &ts[i], 4);
    ptr += 4;
    std::memcpy(ptr, &levels[i], 2);
    ptr += 2;
    std::memcpy(ptr, &lens[i], 2);
    ptr += 2;
  }
  
  // Compute checksum iters times (data doesn't change)
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(out.data(), total_size);
  }
  
  return hash;
}