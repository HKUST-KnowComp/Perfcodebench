#include "interface.h"

#include <cstdint>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  (void)iters;
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t multiplier = 1099511628211ULL;
  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len));
    hash *= multiplier;
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len >> 8));
    hash *= multiplier;
    for (char c : frame) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(c));
      hash *= multiplier;
    }
  }
  return hash;
}
