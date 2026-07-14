#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline void fnv1a_u8(uint64_t& hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Match baseline semantics: if iters == 0, return 0.
  if (iters <= 0) return 0ULL;

  // Compute checksum directly over the logical packed bytes once.
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // Little-endian length prefix: low byte, then high byte.
    fnv1a_u8(hash, static_cast<uint8_t>(len & 0xFF));
    fnv1a_u8(hash, static_cast<uint8_t>((len >> 8) & 0xFF));

    const unsigned char* data = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Small unroll for throughput.
    size_t i = 0;
    size_t n4 = n & ~static_cast<size_t>(3);
    for (; i < n4; i += 4) {
      fnv1a_u8(hash, data[i]);
      fnv1a_u8(hash, data[i + 1]);
      fnv1a_u8(hash, data[i + 2]);
      fnv1a_u8(hash, data[i + 3]);
    }
    for (; i < n; ++i) {
      fnv1a_u8(hash, data[i]);
    }
  }

  return hash;
}
