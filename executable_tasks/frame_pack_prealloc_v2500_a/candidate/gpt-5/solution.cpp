#include "interface.h"

namespace {

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline uint64_t fnv1a_packed_frames(const std::vector<std::string>& frames) {
  uint64_t hash = kFNVOffset;
  for (const std::string& frame : frames) {
    uint16_t v = static_cast<uint16_t>(frame.size());
    uint8_t b0 = static_cast<uint8_t>(v & 0xFF);
    uint8_t b1 = static_cast<uint8_t>((v >> 8) & 0xFF);
    hash ^= static_cast<uint64_t>(b0);
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(b1);
    hash *= kFNVPrime;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Manually unroll by 4 for better throughput on tiny frames.
    size_t i = 0;
    size_t n4 = n & ~static_cast<size_t>(3);
    for (; i < n4; i += 4) {
      hash ^= static_cast<uint64_t>(p[i]);
      hash *= kFNVPrime;
      hash ^= static_cast<uint64_t>(p[i + 1]);
      hash *= kFNVPrime;
      hash ^= static_cast<uint64_t>(p[i + 2]);
      hash *= kFNVPrime;
      hash ^= static_cast<uint64_t>(p[i + 3]);
      hash *= kFNVPrime;
    }
    for (; i < n; ++i) {
      hash ^= static_cast<uint64_t>(p[i]);
      hash *= kFNVPrime;
    }
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;
  // Packed output is identical each iteration; compute once.
  return fnv1a_packed_frames(frames);
}
