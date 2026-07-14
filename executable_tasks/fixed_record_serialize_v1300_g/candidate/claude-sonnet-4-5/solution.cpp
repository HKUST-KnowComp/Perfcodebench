#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline void write_u32_direct(uint8_t* ptr, uint32_t v) {
  std::memcpy(ptr, &v, 4);
}

inline void write_u16_direct(uint8_t* ptr, uint16_t v) {
  std::memcpy(ptr, &v, 2);
}

uint64_t checksum_inline(const uint8_t* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  const std::size_t n = ids.size();
  const std::size_t record_size = 12;  // 4 + 4 + 2 + 2
  const std::size_t total_size = n * record_size;
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      write_u32_direct(ptr, ids[i]);
      ptr += 4;
      write_u32_direct(ptr, ts[i]);
      ptr += 4;
      write_u16_direct(ptr, levels[i]);
      ptr += 2;
      write_u16_direct(ptr, lens[i]);
      ptr += 2;
    }
    
    hash = checksum_inline(out.data(), total_size);
  }
  
  return hash;
}