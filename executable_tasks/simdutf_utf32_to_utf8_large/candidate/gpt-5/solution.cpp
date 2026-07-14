#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  const char32_t* p = input.data();
  const char32_t* end = p + input.size();
  uint64_t hash = FNV_OFFSET;

  while (p < end) {
    // Fast path: consume runs of ASCII code points
    if (*p <= 0x7F) {
      do {
        uint8_t b = static_cast<uint8_t>(*p);
        hash ^= b;
        hash *= FNV_PRIME;
        ++p;
      } while (p < end && *p <= 0x7F);
      continue;
    }

    uint32_t cp = static_cast<uint32_t>(*p);
    ++p;

    if (cp <= 0x7FFu) {
      uint8_t b1 = static_cast<uint8_t>(0xC0u | (cp >> 6));
      uint8_t b2 = static_cast<uint8_t>(0x80u | (cp & 0x3Fu));
      hash ^= b1; hash *= FNV_PRIME;
      hash ^= b2; hash *= FNV_PRIME;
    } else if (cp <= 0xFFFFu) {
      uint8_t b1 = static_cast<uint8_t>(0xE0u | (cp >> 12));
      uint8_t b2 = static_cast<uint8_t>(0x80u | ((cp >> 6) & 0x3Fu));
      uint8_t b3 = static_cast<uint8_t>(0x80u | (cp & 0x3Fu));
      hash ^= b1; hash *= FNV_PRIME;
      hash ^= b2; hash *= FNV_PRIME;
      hash ^= b3; hash *= FNV_PRIME;
    } else {
      // Valid UTF-32 input guarantees cp <= 0x10FFFF and not surrogate range
      uint8_t b1 = static_cast<uint8_t>(0xF0u | (cp >> 18));
      uint8_t b2 = static_cast<uint8_t>(0x80u | ((cp >> 12) & 0x3Fu));
      uint8_t b3 = static_cast<uint8_t>(0x80u | ((cp >> 6) & 0x3Fu));
      uint8_t b4 = static_cast<uint8_t>(0x80u | (cp & 0x3Fu));
      hash ^= b1; hash *= FNV_PRIME;
      hash ^= b2; hash *= FNV_PRIME;
      hash ^= b3; hash *= FNV_PRIME;
      hash ^= b4; hash *= FNV_PRIME;
    }
  }

  return hash;
}
