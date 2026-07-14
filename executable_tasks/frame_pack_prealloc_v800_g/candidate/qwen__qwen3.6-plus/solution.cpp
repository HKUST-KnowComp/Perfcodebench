#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      hash ^= static_cast<uint64_t>(len & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(len >> 8);
      hash *= FNV_PRIME;
      
      const unsigned char* data = reinterpret_cast<const unsigned char*>(frame.data());
      size_t n = frame.size();
      for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
      }
    }
  }
  return hash;
}
