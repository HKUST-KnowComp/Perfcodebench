#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  const uint64_t offset = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = offset;
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // write little-endian length bytes
      uint8_t len0 = static_cast<uint8_t>(len);
      uint8_t len1 = static_cast<uint8_t>(len >> 8);
      hash ^= len0;
      hash *= prime;
      hash ^= len1;
      hash *= prime;
      // process frame bytes
      for (uint8_t c : frame) {
        hash ^= c;
        hash *= prime;
      }
    }
  }
  return hash;
}