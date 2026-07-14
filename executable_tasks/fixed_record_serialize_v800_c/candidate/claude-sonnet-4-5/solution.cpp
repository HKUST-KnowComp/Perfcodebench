#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline void write_u32_direct(uint8_t*& ptr, uint32_t v) {
  std::memcpy(ptr, &v, 4);
  ptr += 4;
}

inline void write_u16_direct(uint8_t*& ptr, uint16_t v) {
  std::memcpy(ptr, &v, 2);
  ptr += 2;
}

inline uint64_t update_hash(uint64_t hash, const uint8_t* ptr, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
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
  const size_t record_size = 12;  // 4 + 4 + 2 + 2
  const size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    size_t i = 0;
    // Unroll by 4
    for (; i + 3 < n; i += 4) {
      write_u32_direct(ptr, ids[i]);
      write_u32_direct(ptr, ts[i]);
      write_u16_direct(ptr, levels[i]);
      write_u16_direct(ptr, lens[i]);
      
      write_u32_direct(ptr, ids[i+1]);
      write_u32_direct(ptr, ts[i+1]);
      write_u16_direct(ptr, levels[i+1]);
      write_u16_direct(ptr, lens[i+1]);
      
      write_u32_direct(ptr, ids[i+2]);
      write_u32_direct(ptr, ts[i+2]);
      write_u16_direct(ptr, levels[i+2]);
      write_u16_direct(ptr, lens[i+2]);
      
      write_u32_direct(ptr, ids[i+3]);
      write_u32_direct(ptr, ts[i+3]);
      write_u16_direct(ptr, levels[i+3]);
      write_u16_direct(ptr, lens[i+3]);
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      write_u32_direct(ptr, ids[i]);
      write_u32_direct(ptr, ts[i]);
      write_u16_direct(ptr, levels[i]);
      write_u16_direct(ptr, lens[i]);
    }
    
    // Compute checksum
    hash = 1469598103934665603ULL;
    hash = update_hash(hash, out.data(), total_size);
  }
  
  return hash;
}