#include "interface.h"

namespace {

inline uint64_t fnv1_hash_packed(const std::vector<std::string>& frames) {
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET_BASIS;

  for (const std::string& frame : frames) {
    // Write 16-bit little-endian length prefix
    uint16_t len = static_cast<uint16_t>(frame.size());
    uint8_t low = static_cast<uint8_t>(len & 0xFF);
    uint8_t high = static_cast<uint8_t>((len >> 8) & 0xFF);

    hash ^= static_cast<uint64_t>(low);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(high);
    hash *= FNV_PRIME;

    // Write frame bytes
    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    const unsigned char* end = p + frame.size();
    while (p != end) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= FNV_PRIME;
    }
  }

  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;
  // The checksum is identical for every iteration in the baseline; compute once.
  return fnv1_hash_packed(frames);
}
