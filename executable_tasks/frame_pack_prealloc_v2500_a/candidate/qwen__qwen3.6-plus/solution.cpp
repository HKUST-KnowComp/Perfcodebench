#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const auto& f : frames) {
    total_size += f.size() + 2;
  }

  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    hash = 1469598103934665603ULL;
    for (const auto& f : frames) {
      uint16_t len = static_cast<uint16_t>(f.size());
      uint8_t b0 = static_cast<uint8_t>(len);
      uint8_t b1 = static_cast<uint8_t>(len >> 8);
      
      hash ^= b0; hash *= 1099511628211ULL;
      *ptr++ = b0;
      hash ^= b1; hash *= 1099511628211ULL;
      *ptr++ = b1;
      
      for (char c : f) {
        uint8_t b = static_cast<uint8_t>(c);
        hash ^= b; hash *= 1099511628211ULL;
        *ptr++ = b;
      }
    }
  }
  return hash;
}
