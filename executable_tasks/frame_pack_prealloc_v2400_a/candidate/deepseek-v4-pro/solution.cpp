#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  for (const std::string& f : frames) {
    uint16_t len = static_cast<uint16_t>(f.size());
    // low byte
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len));
    hash *= 1099511628211ULL;
    // high byte
    hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len >> 8));
    hash *= 1099511628211ULL;
    for (char c : f) {
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(c));
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}