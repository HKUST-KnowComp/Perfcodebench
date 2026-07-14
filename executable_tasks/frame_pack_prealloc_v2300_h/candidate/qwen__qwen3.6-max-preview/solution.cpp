#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (const std::string& frame : frames) {
      size_t sz = frame.size();
      uint16_t len = static_cast<uint16_t>(sz);
      uint8_t b0 = static_cast<uint8_t>(len);
      uint8_t b1 = static_cast<uint8_t>(len >> 8);
      
      h ^= b0; h *= 1099511628211ULL;
      h ^= b1; h *= 1099511628211ULL;
      
      const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
      for (size_t i = 0; i < sz; ++i) {
        h ^= data[i];
        h *= 1099511628211ULL;
      }
    }
    hash = h;
  }
  return hash;
}
