#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, uint8_t byte) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  hash ^= byte;
  hash *= kPrime;
  return hash;
}

static inline uint64_t fnv1a_bytes(uint64_t hash, const unsigned char* p, size_t n) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  while (n >= 8) {
    hash ^= p[0]; hash *= kPrime;
    hash ^= p[1]; hash *= kPrime;
    hash ^= p[2]; hash *= kPrime;
    hash ^= p[3]; hash *= kPrime;
    hash ^= p[4]; hash *= kPrime;
    hash ^= p[5]; hash *= kPrime;
    hash ^= p[6]; hash *= kPrime;
    hash ^= p[7]; hash *= kPrime;
    p += 8; n -= 8;
  }
  while (n--) {
    hash ^= *p++;
    hash *= kPrime;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  for (const std::string& frame : frames) {
    uint16_t len16 = static_cast<uint16_t>(frame.size());
    // Write length prefix (little-endian) into the hash stream
    hash = fnv1a_update(hash, static_cast<uint8_t>(len16 & 0xFF));
    hash = fnv1a_update(hash, static_cast<uint8_t>(len16 >> 8));

    const unsigned char* data = reinterpret_cast<const unsigned char*>(frame.data());
    hash = fnv1a_bytes(hash, data, frame.size());
  }

  return hash;
}
