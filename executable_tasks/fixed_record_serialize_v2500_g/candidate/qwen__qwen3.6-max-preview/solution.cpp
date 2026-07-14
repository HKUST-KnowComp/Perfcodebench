#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  size_t n = ids.size();
  std::vector<uint8_t> out(n * 12);
  uint64_t hash = 0;
  constexpr uint64_t fnv_prime = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    for (size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      ptr[0] = static_cast<uint8_t>(id);
      ptr[1] = static_cast<uint8_t>(id >> 8);
      ptr[2] = static_cast<uint8_t>(id >> 16);
      ptr[3] = static_cast<uint8_t>(id >> 24);
      ptr += 4;
      
      uint32_t t = ts[i];
      ptr[0] = static_cast<uint8_t>(t);
      ptr[1] = static_cast<uint8_t>(t >> 8);
      ptr[2] = static_cast<uint8_t>(t >> 16);
      ptr[3] = static_cast<uint8_t>(t >> 24);
      ptr += 4;
      
      uint16_t lv = levels[i];
      ptr[0] = static_cast<uint8_t>(lv);
      ptr[1] = static_cast<uint8_t>(lv >> 8);
      ptr += 2;
      
      uint16_t ln = lens[i];
      ptr[0] = static_cast<uint8_t>(ln);
      ptr[1] = static_cast<uint8_t>(ln >> 8);
      ptr += 2;
    }
    
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* data = out.data();
    const uint8_t* end = data + out.size();
    while (data < end) {
      h ^= *data++;
      h *= fnv_prime;
    }
    hash = h;
  }
  return hash;
}