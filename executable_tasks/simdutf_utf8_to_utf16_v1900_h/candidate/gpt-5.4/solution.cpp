#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t fnv1a_append(uint64_t hash, char16_t ch) {
  hash ^= static_cast<uint64_t>(ch);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* data = input.data();
  const size_t len = input.size();

  uint64_t hash = 1469598103934665603ULL;

  // Fast path for ASCII runs: UTF-8 bytes map directly to identical UTF-16 code units.
  size_t i = 0;
  while (i < len) {
    while (i < len && static_cast<unsigned char>(data[i]) <= 0x7F) {
      hash = fnv1a_append(hash, static_cast<char16_t>(static_cast<unsigned char>(data[i])));
      ++i;
    }
    if (i >= len) break;

    // Decode one non-ASCII code point. Input is guaranteed valid UTF-8.
    const unsigned char c0 = static_cast<unsigned char>(data[i++]);
    uint32_t cp;
    if (c0 < 0xE0) {
      cp = (static_cast<uint32_t>(c0 & 0x1FU) << 6) |
           static_cast<uint32_t>(static_cast<unsigned char>(data[i++]) & 0x3FU);
      hash = fnv1a_append(hash, static_cast<char16_t>(cp));
    } else if (c0 < 0xF0) {
      cp = (static_cast<uint32_t>(c0 & 0x0FU) << 12) |
           (static_cast<uint32_t>(static_cast<unsigned char>(data[i]) & 0x3FU) << 6) |
           static_cast<uint32_t>(static_cast<unsigned char>(data[i + 1]) & 0x3FU);
      i += 2;
      hash = fnv1a_append(hash, static_cast<char16_t>(cp));
    } else {
      cp = (static_cast<uint32_t>(c0 & 0x07U) << 18) |
           (static_cast<uint32_t>(static_cast<unsigned char>(data[i]) & 0x3FU) << 12) |
           (static_cast<uint32_t>(static_cast<unsigned char>(data[i + 1]) & 0x3FU) << 6) |
           static_cast<uint32_t>(static_cast<unsigned char>(data[i + 2]) & 0x3FU);
      i += 3;
      cp -= 0x10000U;
      hash = fnv1a_append(hash, static_cast<char16_t>(0xD800U + (cp >> 10)));
      hash = fnv1a_append(hash, static_cast<char16_t>(0xDC00U + (cp & 0x3FFU)));
    }
  }

  return hash;
}
