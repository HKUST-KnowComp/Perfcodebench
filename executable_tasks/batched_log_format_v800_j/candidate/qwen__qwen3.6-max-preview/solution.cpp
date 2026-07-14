#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = 0;
  
  char buf[16];
  const size_t n = ids.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (size_t i = 0; i < n; ++i) {
      // Timestamp
      uint32_t v = timestamps[i];
      if (v == 0) {
        hash ^= '0'; hash *= FNV_PRIME;
      } else {
        int len = 0;
        while (v) { buf[len++] = '0' + (v % 10); v /= 10; }
        while (len--) { hash ^= (uint8_t)buf[len]; hash *= FNV_PRIME; }
      }
      hash ^= '|'; hash *= FNV_PRIME;
      
      // ID
      v = ids[i];
      if (v == 0) {
        hash ^= '0'; hash *= FNV_PRIME;
      } else {
        int len = 0;
        while (v) { buf[len++] = '0' + (v % 10); v /= 10; }
        while (len--) { hash ^= (uint8_t)buf[len]; hash *= FNV_PRIME; }
      }
      hash ^= '|'; hash *= FNV_PRIME;
      
      // Level
      uint32_t lv = levels[i];
      if (lv == 0) {
        hash ^= '0'; hash *= FNV_PRIME;
      } else {
        int len = 0;
        while (lv) { buf[len++] = '0' + (lv % 10); lv /= 10; }
        while (len--) { hash ^= (uint8_t)buf[len]; hash *= FNV_PRIME; }
      }
      hash ^= '|'; hash *= FNV_PRIME;
      
      // Message
      const char* p = messages[i].data();
      const char* end = p + messages[i].size();
      while (p != end) {
        hash ^= (uint8_t)*p++;
        hash *= FNV_PRIME;
      }
      
      // Newline
      hash ^= '\n'; hash *= FNV_PRIME;
    }
  }
  return hash;
}