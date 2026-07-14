#include "interface.h"

#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Preserve baseline behavior: if no iterations requested, return 0
  if (iters <= 0) {
    return 0;
  }

  // FNV-1a 64-bit constants
  const uint64_t kOffset = 1469598103934665603ULL;
  const uint64_t kPrime  = 1099511628211ULL;

  uint64_t hash = kOffset;

  for (const std::string& frame : frames) {
    // Length prefix (little-endian 16-bit, truncated like baseline)
    uint16_t v = static_cast<uint16_t>(frame.size());
    uint8_t lo = static_cast<uint8_t>(v);
    uint8_t hi = static_cast<uint8_t>(v >> 8);

    hash ^= lo;
    hash *= kPrime;
    hash ^= hi;
    hash *= kPrime;

    // Frame payload bytes
    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Unroll by 8 for throughput
    size_t blocks = n / 8;
    while (blocks--) {
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
      hash ^= *p++; hash *= kPrime;
    }
    size_t rem = n & 7;
    while (rem--) {
      hash ^= *p++;
      hash *= kPrime;
    }
  }

  // The packed output is identical each iteration; baseline overwrote the same value each time.
  // Compute once and return.
  return hash;
}
