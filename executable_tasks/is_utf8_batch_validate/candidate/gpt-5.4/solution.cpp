#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

inline bool valid_scalar(const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();

  while (p < end) {
    // Fast ASCII run.
    while (p < end && *p <= 0x7F) {
      ++p;
    }
    if (p >= end) return true;

    const unsigned char c = *p++;

    if (c < 0xC2) return false;

    if (c <= 0xDF) {
      if (p >= end) return false;
      if ((p[0] & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }

    if (c <= 0xEF) {
      if (end - p < 2) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      if ((b2 & 0xC0U) != 0x80U) return false;

      if (c == 0xE0) {
        if (b1 < 0xA0 || b1 > 0xBF) return false;
      } else if (c == 0xED) {
        if (b1 < 0x80 || b1 > 0x9F) return false;
      } else {
        if ((b1 & 0xC0U) != 0x80U) return false;
      }
      p += 2;
      continue;
    }

    if (c <= 0xF4) {
      if (end - p < 3) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      const unsigned char b3 = p[2];
      if (((b2 & 0xC0U) != 0x80U) || ((b3 & 0xC0U) != 0x80U)) return false;

      if (c == 0xF0) {
        if (b1 < 0x90 || b1 > 0xBF) return false;
      } else if (c == 0xF4) {
        if (b1 < 0x80 || b1 > 0x8F) return false;
      } else {
        if ((b1 & 0xC0U) != 0x80U) return false;
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
    count += static_cast<std::size_t>(valid_scalar(s));
  }

  return count;
}
