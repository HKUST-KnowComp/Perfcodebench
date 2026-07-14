#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline bool validate_utf8_fast(const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();

  while (p < end) {
    // Fast ASCII skip: process machine words while all bytes are ASCII.
    while (static_cast<std::size_t>(end - p) >= sizeof(std::size_t)) {
      std::size_t word = 0;
      __builtin_memcpy(&word, p, sizeof(word));
      if (word & (sizeof(std::size_t) == 8
                      ? static_cast<std::size_t>(0x8080808080808080ULL)
                      : static_cast<std::size_t>(0x80808080U))) {
        break;
      }
      p += sizeof(std::size_t);
    }
    while (p < end && *p <= 0x7F) {
      ++p;
    }
    if (p >= end) break;

    const unsigned char c = *p++;

    if (c < 0xC2) {
      return false;
    }

    if (c <= 0xDF) {
      if (p >= end || (p[0] & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }

    if (c <= 0xEF) {
      if (static_cast<std::size_t>(end - p) < 2) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U) return false;
      if (c == 0xE0) {
        if (b1 < 0xA0 || b1 > 0xBF) return false;
      } else if (c == 0xED) {
        if (b1 > 0x9F) return false;
      }
      p += 2;
      continue;
    }

    if (c <= 0xF4) {
      if (static_cast<std::size_t>(end - p) < 3) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      const unsigned char b3 = p[2];
      if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
      if (c == 0xF0) {
        if (b1 < 0x90 || b1 > 0xBF) return false;
      } else if (c == 0xF4) {
        if (b1 > 0x8F) return false;
      }
      p += 3;
      continue;
    }

    return false;
  }

  return true;
}

}  // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    count += static_cast<std::size_t>(validate_utf8_fast(s));
  }
  return count;
}
