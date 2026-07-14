#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  constexpr uint64_t kSeed = 1469598103934665603ULL;
  constexpr uint64_t kMul = 1099511628211ULL;
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = kSeed;

    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      // Emit length bytes (little-endian) into hash stream
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len & 0xFF));
      hash *= kMul;
      hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len >> 8));
      hash *= kMul;

      // Emit frame data bytes into hash stream
      for (unsigned char c : frame) {
        hash ^= static_cast<uint64_t>(c);
        hash *= kMul;
      }
    }

    final_hash = hash;
  }

  return final_hash;
}
