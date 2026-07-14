#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;

  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  uint64_t hash = fnv_offset_basis;

  for (const std::string& frame : frames) {
    uint16_t v = static_cast<uint16_t>(frame.size());
    uint8_t b0 = static_cast<uint8_t>(v);
    uint8_t b1 = static_cast<uint8_t>(v >> 8);

    hash ^= static_cast<uint64_t>(b0);
    hash *= fnv_prime;
    hash ^= static_cast<uint64_t>(b1);
    hash *= fnv_prime;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    const unsigned char* end = p + frame.size();

    // Unroll by 4 for better throughput
    while (p + 4 <= end) {
      hash ^= static_cast<uint64_t>(p[0]); hash *= fnv_prime;
      hash ^= static_cast<uint64_t>(p[1]); hash *= fnv_prime;
      hash ^= static_cast<uint64_t>(p[2]); hash *= fnv_prime;
      hash ^= static_cast<uint64_t>(p[3]); hash *= fnv_prime;
      p += 4;
    }
    while (p < end) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= fnv_prime;
    }
  }

  return hash;
}
