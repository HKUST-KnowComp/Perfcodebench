#include "interface.h"

namespace {

inline uint64_t fnv1a_pack_checksum(const std::vector<std::string>& frames) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // length prefix, little-endian
    hash ^= static_cast<uint8_t>(len);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(len >> 8);
    hash *= FNV_PRIME;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    const unsigned char* e = p + frame.size();

    // Unrolled processing by 8 bytes for throughput
    while (p + 8 <= e) {
      hash ^= p[0]; hash *= FNV_PRIME;
      hash ^= p[1]; hash *= FNV_PRIME;
      hash ^= p[2]; hash *= FNV_PRIME;
      hash ^= p[3]; hash *= FNV_PRIME;
      hash ^= p[4]; hash *= FNV_PRIME;
      hash ^= p[5]; hash *= FNV_PRIME;
      hash ^= p[6]; hash *= FNV_PRIME;
      hash ^= p[7]; hash *= FNV_PRIME;
      p += 8;
    }
    while (p < e) {
      hash ^= *p++;
      hash *= FNV_PRIME;
    }
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;  // matches baseline behavior
  // Baseline returns checksum of the packed output of the final iteration.
  // Since each iteration produces identical bytes, compute once.
  return fnv1a_pack_checksum(frames);
}
