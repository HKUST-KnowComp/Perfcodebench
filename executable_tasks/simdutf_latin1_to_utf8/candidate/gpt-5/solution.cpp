#include "interface.h"

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void fnv1a_update(uint64_t &hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= FNV_PRIME;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* end = p + input.size();

  uint64_t hash = FNV_OFFSET_BASIS;

  while (p < end) {
    // Fast path: process a run of ASCII bytes (0x00..0x7F)
    while (p < end && (*p < 0x80)) {
      // Small unroll to reduce loop control overhead on long ASCII runs
      if (p + 4 <= end && p[0] < 0x80 && p[1] < 0x80 && p[2] < 0x80 && p[3] < 0x80) {
        fnv1a_update(hash, p[0]);
        fnv1a_update(hash, p[1]);
        fnv1a_update(hash, p[2]);
        fnv1a_update(hash, p[3]);
        p += 4;
        continue;
      }
      fnv1a_update(hash, *p++);
    }

    // Non-ASCII byte: Latin-1 to UTF-8 two-byte sequence
    if (p < end) {
      unsigned char ch = *p++;
      unsigned char b1 = static_cast<unsigned char>(0xC0u | (ch >> 6));
      unsigned char b2 = static_cast<unsigned char>(0x80u | (ch & 0x3Fu));
      fnv1a_update(hash, b1);
      fnv1a_update(hash, b2);
    }
  }

  return hash;
}
