#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  const size_t n = ids.size();
  const size_t total_bytes = n * 12;
  
  std::vector<uint8_t> out(total_bytes);
  uint8_t* buf = out.data();
  
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = buf;
    hash = FNV_OFFSET;
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t v32 = ids[i];
      hash ^= v32 & 0xFF; hash *= FNV_PRIME; *p++ = v32 & 0xFF;
      hash ^= (v32 >> 8) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 8) & 0xFF;
      hash ^= (v32 >> 16) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 16) & 0xFF;
      hash ^= (v32 >> 24) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 24) & 0xFF;
      
      v32 = ts[i];
      hash ^= v32 & 0xFF; hash *= FNV_PRIME; *p++ = v32 & 0xFF;
      hash ^= (v32 >> 8) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 8) & 0xFF;
      hash ^= (v32 >> 16) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 16) & 0xFF;
      hash ^= (v32 >> 24) & 0xFF; hash *= FNV_PRIME; *p++ = (v32 >> 24) & 0xFF;
      
      uint16_t v16 = levels[i];
      hash ^= v16 & 0xFF; hash *= FNV_PRIME; *p++ = v16 & 0xFF;
      hash ^= (v16 >> 8) & 0xFF; hash *= FNV_PRIME; *p++ = (v16 >> 8) & 0xFF;
      
      v16 = lens[i];
      hash ^= v16 & 0xFF; hash *= FNV_PRIME; *p++ = v16 & 0xFF;
      hash ^= (v16 >> 8) & 0xFF; hash *= FNV_PRIME; *p++ = (v16 >> 8) & 0xFF;
    }
  }
  return hash;
}