#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  const uint64_t seed = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;
  uint64_t hash = seed;

  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // little-endian length prefix (two bytes)
    hash ^= static_cast<uint8_t>(len);
    hash *= mul;
    hash ^= static_cast<uint8_t>(len >> 8);
    hash *= mul;

    for (char c : frame) {
      hash ^= static_cast<uint8_t>(c);
      hash *= mul;
    }
  }

  return hash;
}