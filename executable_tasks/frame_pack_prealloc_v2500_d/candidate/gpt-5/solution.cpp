#include "interface.h"

namespace {
static inline uint64_t fnv1a_lenprefixed_checksum(const std::vector<std::string>& frames) {
  const uint64_t kOffset = 1469598103934665603ULL;
  const uint64_t kPrime = 1099511628211ULL;
  uint64_t hash = kOffset;

  for (const std::string& frame : frames) {
    uint16_t v = static_cast<uint16_t>(frame.size());
    uint8_t b0 = static_cast<uint8_t>(v);
    uint8_t b1 = static_cast<uint8_t>(v >> 8);

    hash ^= static_cast<uint64_t>(b0);
    hash *= kPrime;
    hash ^= static_cast<uint64_t>(b1);
    hash *= kPrime;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    const unsigned char* e = p + frame.size();
    while (p != e) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= kPrime;
    }
  }
  return hash;
}
}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;
  return fnv1a_lenprefixed_checksum(frames);
}
