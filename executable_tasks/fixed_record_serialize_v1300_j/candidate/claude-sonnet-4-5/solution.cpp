#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline void write_u32(uint8_t*& ptr, uint32_t v) {
  std::memcpy(ptr, &v, 4);
  ptr += 4;
}

inline void write_u16(uint8_t*& ptr, uint16_t v) {
  std::memcpy(ptr, &v, 2);
  ptr += 2;
}

uint64_t checksum_bytes_fast(const uint8_t* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  std::size_t i = 0;
  const std::size_t unroll_limit = size - (size % 4);
  
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
  
  for (; i < size; ++i) {
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
  const std::size_t n = ids.size();
  const std::size_t bytes_per_record = 12;
  const std::size_t total_size = n * bytes_per_record;
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      write_u32(ptr, ids[i]);
      write_u32(ptr, ts[i]);
      write_u16(ptr, levels[i]);
      write_u16(ptr, lens[i]);
    }
    
    hash = checksum_bytes_fast(out.data(), total_size);
  }
  
  return hash;
}