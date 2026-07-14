#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline void write_u32(uint8_t*& ptr, uint32_t v) {
  std::memcpy(ptr, &v, 4);
  ptr += 4;
}

inline void write_u16(uint8_t*& ptr, uint16_t v) {
  std::memcpy(ptr, &v, 2);
  ptr += 2;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t bytes_per_record = 12;  // 4 + 4 + 2 + 2
  const std::size_t total_bytes = n * bytes_per_record;
  
  std::vector<uint8_t> out(total_bytes);
  uint64_t hash = 0;
  
  const uint32_t* ids_ptr = ids.data();
  const uint32_t* ts_ptr = ts.data();
  const uint16_t* levels_ptr = levels.data();
  const uint16_t* lens_ptr = lens.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      write_u32(ptr, ids_ptr[i]);
      write_u32(ptr, ts_ptr[i]);
      write_u16(ptr, levels_ptr[i]);
      write_u16(ptr, lens_ptr[i]);
    }
    
    // Compute checksum inline
    hash = 1469598103934665603ULL;
    const uint8_t* byte_ptr = out.data();
    for (std::size_t i = 0; i < total_bytes; ++i) {
      hash ^= static_cast<uint64_t>(byte_ptr[i]);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}