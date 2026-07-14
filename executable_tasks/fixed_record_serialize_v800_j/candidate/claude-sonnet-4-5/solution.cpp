#include "interface.h"

#include <cstring>
#include <vector>

namespace {

uint64_t checksum_and_serialize(
    uint8_t* out,
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint8_t* ptr = out;
  
  const std::size_t n = ids.size();
  const std::size_t n_unroll = n & ~3ULL;
  
  for (std::size_t i = 0; i < n_unroll; i += 4) {
    // Record 0
    uint32_t id0 = ids[i];
    uint32_t ts0 = ts[i];
    uint16_t level0 = levels[i];
    uint16_t len0 = lens[i];
    
    ptr[0] = static_cast<uint8_t>(id0);
    ptr[1] = static_cast<uint8_t>(id0 >> 8);
    ptr[2] = static_cast<uint8_t>(id0 >> 16);
    ptr[3] = static_cast<uint8_t>(id0 >> 24);
    ptr[4] = static_cast<uint8_t>(ts0);
    ptr[5] = static_cast<uint8_t>(ts0 >> 8);
    ptr[6] = static_cast<uint8_t>(ts0 >> 16);
    ptr[7] = static_cast<uint8_t>(ts0 >> 24);
    ptr[8] = static_cast<uint8_t>(level0);
    ptr[9] = static_cast<uint8_t>(level0 >> 8);
    ptr[10] = static_cast<uint8_t>(len0);
    ptr[11] = static_cast<uint8_t>(len0 >> 8);
    
    hash ^= ptr[0]; hash *= prime;
    hash ^= ptr[1]; hash *= prime;
    hash ^= ptr[2]; hash *= prime;
    hash ^= ptr[3]; hash *= prime;
    hash ^= ptr[4]; hash *= prime;
    hash ^= ptr[5]; hash *= prime;
    hash ^= ptr[6]; hash *= prime;
    hash ^= ptr[7]; hash *= prime;
    hash ^= ptr[8]; hash *= prime;
    hash ^= ptr[9]; hash *= prime;
    hash ^= ptr[10]; hash *= prime;
    hash ^= ptr[11]; hash *= prime;
    ptr += 12;
    
    // Record 1
    uint32_t id1 = ids[i + 1];
    uint32_t ts1 = ts[i + 1];
    uint16_t level1 = levels[i + 1];
    uint16_t len1 = lens[i + 1];
    
    ptr[0] = static_cast<uint8_t>(id1);
    ptr[1] = static_cast<uint8_t>(id1 >> 8);
    ptr[2] = static_cast<uint8_t>(id1 >> 16);
    ptr[3] = static_cast<uint8_t>(id1 >> 24);
    ptr[4] = static_cast<uint8_t>(ts1);
    ptr[5] = static_cast<uint8_t>(ts1 >> 8);
    ptr[6] = static_cast<uint8_t>(ts1 >> 16);
    ptr[7] = static_cast<uint8_t>(ts1 >> 24);
    ptr[8] = static_cast<uint8_t>(level1);
    ptr[9] = static_cast<uint8_t>(level1 >> 8);
    ptr[10] = static_cast<uint8_t>(len1);
    ptr[11] = static_cast<uint8_t>(len1 >> 8);
    
    hash ^= ptr[0]; hash *= prime;
    hash ^= ptr[1]; hash *= prime;
    hash ^= ptr[2]; hash *= prime;
    hash ^= ptr[3]; hash *= prime;
    hash ^= ptr[4]; hash *= prime;
    hash ^= ptr[5]; hash *= prime;
    hash ^= ptr[6]; hash *= prime;
    hash ^= ptr[7]; hash *= prime;
    hash ^= ptr[8]; hash *= prime;
    hash ^= ptr[9]; hash *= prime;
    hash ^= ptr[10]; hash *= prime;
    hash ^= ptr[11]; hash *= prime;
    ptr += 12;
    
    // Record 2
    uint32_t id2 = ids[i + 2];
    uint32_t ts2 = ts[i + 2];
    uint16_t level2 = levels[i + 2];
    uint16_t len2 = lens[i + 2];
    
    ptr[0] = static_cast<uint8_t>(id2);
    ptr[1] = static_cast<uint8_t>(id2 >> 8);
    ptr[2] = static_cast<uint8_t>(id2 >> 16);
    ptr[3] = static_cast<uint8_t>(id2 >> 24);
    ptr[4] = static_cast<uint8_t>(ts2);
    ptr[5] = static_cast<uint8_t>(ts2 >> 8);
    ptr[6] = static_cast<uint8_t>(ts2 >> 16);
    ptr[7] = static_cast<uint8_t>(ts2 >> 24);
    ptr[8] = static_cast<uint8_t>(level2);
    ptr[9] = static_cast<uint8_t>(level2 >> 8);
    ptr[10] = static_cast<uint8_t>(len2);
    ptr[11] = static_cast<uint8_t>(len2 >> 8);
    
    hash ^= ptr[0]; hash *= prime;
    hash ^= ptr[1]; hash *= prime;
    hash ^= ptr[2]; hash *= prime;
    hash ^= ptr[3]; hash *= prime;
    hash ^= ptr[4]; hash *= prime;
    hash ^= ptr[5]; hash *= prime;
    hash ^= ptr[6]; hash *= prime;
    hash ^= ptr[7]; hash *= prime;
    hash ^= ptr[8]; hash *= prime;
    hash ^= ptr[9]; hash *= prime;
    hash ^= ptr[10]; hash *= prime;
    hash ^= ptr[11]; hash *= prime;
    ptr += 12;
    
    // Record 3
    uint32_t id3 = ids[i + 3];
    uint32_t ts3 = ts[i + 3];
    uint16_t level3 = levels[i + 3];
    uint16_t len3 = lens[i + 3];
    
    ptr[0] = static_cast<uint8_t>(id3);
    ptr[1] = static_cast<uint8_t>(id3 >> 8);
    ptr[2] = static_cast<uint8_t>(id3 >> 16);
    ptr[3] = static_cast<uint8_t>(id3 >> 24);
    ptr[4] = static_cast<uint8_t>(ts3);
    ptr[5] = static_cast<uint8_t>(ts3 >> 8);
    ptr[6] = static_cast<uint8_t>(ts3 >> 16);
    ptr[7] = static_cast<uint8_t>(ts3 >> 24);
    ptr[8] = static_cast<uint8_t>(level3);
    ptr[9] = static_cast<uint8_t>(level3 >> 8);
    ptr[10] = static_cast<uint8_t>(len3);
    ptr[11] = static_cast<uint8_t>(len3 >> 8);
    
    hash ^= ptr[0]; hash *= prime;
    hash ^= ptr[1]; hash *= prime;
    hash ^= ptr[2]; hash *= prime;
    hash ^= ptr[3]; hash *= prime;
    hash ^= ptr[4]; hash *= prime;
    hash ^= ptr[5]; hash *= prime;
    hash ^= ptr[6]; hash *= prime;
    hash ^= ptr[7]; hash *= prime;
    hash ^= ptr[8]; hash *= prime;
    hash ^= ptr[9]; hash *= prime;
    hash ^= ptr[10]; hash *= prime;
    hash ^= ptr[11]; hash *= prime;
    ptr += 12;
  }
  
  // Handle remaining records
  for (std::size_t i = n_unroll; i < n; ++i) {
    uint32_t id = ids[i];
    uint32_t ts_val = ts[i];
    uint16_t level = levels[i];
    uint16_t len = lens[i];
    
    ptr[0] = static_cast<uint8_t>(id);
    ptr[1] = static_cast<uint8_t>(id >> 8);
    ptr[2] = static_cast<uint8_t>(id >> 16);
    ptr[3] = static_cast<uint8_t>(id >> 24);
    ptr[4] = static_cast<uint8_t>(ts_val);
    ptr[5] = static_cast<uint8_t>(ts_val >> 8);
    ptr[6] = static_cast<uint8_t>(ts_val >> 16);
    ptr[7] = static_cast<uint8_t>(ts_val >> 24);
    ptr[8] = static_cast<uint8_t>(level);
    ptr[9] = static_cast<uint8_t>(level >> 8);
    ptr[10] = static_cast<uint8_t>(len);
    ptr[11] = static_cast<uint8_t>(len >> 8);
    
    hash ^= ptr[0]; hash *= prime;
    hash ^= ptr[1]; hash *= prime;
    hash ^= ptr[2]; hash *= prime;
    hash ^= ptr[3]; hash *= prime;
    hash ^= ptr[4]; hash *= prime;
    hash ^= ptr[5]; hash *= prime;
    hash ^= ptr[6]; hash *= prime;
    hash ^= ptr[7]; hash *= prime;
    hash ^= ptr[8]; hash *= prime;
    hash ^= ptr[9]; hash *= prime;
    hash ^= ptr[10]; hash *= prime;
    hash ^= ptr[11]; hash *= prime;
    ptr += 12;
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
  const std::size_t record_size = 12;  // 4 + 4 + 2 + 2
  const std::size_t buffer_size = ids.size() * record_size;
  std::vector<uint8_t> out(buffer_size);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_and_serialize(out.data(), ids, ts, levels, lens);
  }
  return hash;
}
