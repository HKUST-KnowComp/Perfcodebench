#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline void write_u32(uint8_t* out, uint32_t v) {
  std::memcpy(out, &v, 4);
}

inline void write_u16(uint8_t* out, uint16_t v) {
  std::memcpy(out, &v, 2);
}

uint64_t checksum_direct(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  const std::size_t n = ids.size();
  const std::size_t record_size = 12;
  const std::size_t total_size = n * record_size;
  
  std::vector<uint8_t> buf(total_size);
  uint8_t* ptr = buf.data();
  
  for (std::size_t i = 0; i < n; ++i) {
    write_u32(ptr, ids[i]);
    write_u32(ptr + 4, ts[i]);
    write_u16(ptr + 8, levels[i]);
    write_u16(ptr + 10, lens[i]);
    ptr += record_size;
  }
  
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  const uint8_t* data = buf.data();
  
  std::size_t i = 0;
  const std::size_t unroll = total_size - (total_size % 8);
  
  for (; i < unroll; i += 8) {
    hash ^= data[i];
    hash *= prime;
    hash ^= data[i + 1];
    hash *= prime;
    hash ^= data[i + 2];
    hash *= prime;
    hash ^= data[i + 3];
    hash *= prime;
    hash ^= data[i + 4];
    hash *= prime;
    hash ^= data[i + 5];
    hash *= prime;
    hash ^= data[i + 6];
    hash *= prime;
    hash ^= data[i + 7];
    hash *= prime;
  }
  
  for (; i < total_size; ++i) {
    hash ^= data[i];
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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_direct(ids, ts, levels, lens);
  }
  return hash;
}
