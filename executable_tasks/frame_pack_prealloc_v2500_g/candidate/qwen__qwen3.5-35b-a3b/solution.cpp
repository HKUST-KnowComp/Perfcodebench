#include "interface.h"

#include <vector>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  uint64_t hash = 1469598103934665603ULL;
  for (const std::string& frame : frames) {
    uint16_t len = frame.size();
    // Process low byte of length
    hash ^= static_cast<uint8_t>(len);
    hash *= 1099511628211ULL;
    // Process high byte of length
    hash ^= static_cast<uint8_t>(len >> 8);
    hash *= 1099511628211ULL;
    // Process each byte of the frame
    for (char c : frame) {
      hash ^= static_cast<uint8_t>(c);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}