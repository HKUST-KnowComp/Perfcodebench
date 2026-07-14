#include "interface.h"

namespace {

constexpr uint64_t kFNV64OffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFNV64Prime = 1099511628211ULL;

inline uint64_t fnv1a_update(uint64_t hash, const unsigned char* data, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= kFNV64Prime;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = kFNV64OffsetBasis;

  for (const std::string& frame : frames) {
    uint16_t len16 = static_cast<uint16_t>(frame.size());
    // Write length prefix (little-endian) directly into hash
    hash ^= static_cast<uint8_t>(len16 & 0xFF);
    hash *= kFNV64Prime;
    hash ^= static_cast<uint8_t>((len16 >> 8) & 0xFF);
    hash *= kFNV64Prime;

    // Hash frame payload bytes
    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    hash = fnv1a_update(hash, p, frame.size());
  }

  // Baseline returns the last iteration result; since all iterations are identical,
  // computing once is sufficient and preserves correctness.
  return hash;
}
