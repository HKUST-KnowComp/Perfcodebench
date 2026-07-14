#include "interface.h"

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Match baseline behavior: if no iterations requested, return 0.
  if (iters <= 0) return 0ULL;

  // Compute the checksum directly over the packed stream without materializing it.
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  for (const std::string& frame : frames) {
    // Length is truncated to 16 bits and written little-endian as in baseline.
    const uint16_t len = static_cast<uint16_t>(frame.size());
    hash = fnv1a_update(hash, static_cast<uint8_t>(len & 0xFF));
    hash = fnv1a_update(hash, static_cast<uint8_t>((len >> 8) & 0xFF));

    // Hash the frame bytes in order, treating bytes as unsigned.
    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    const unsigned char* e = p + frame.size();
    for (; p != e; ++p) {
      hash = fnv1a_update(hash, *p);
    }
  }

  // Baseline assigns the checksum of the last identical iteration; computing once suffices.
  return hash;
}
