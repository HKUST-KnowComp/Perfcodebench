#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>

namespace {

static inline bool valid_utf8_scalar(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* const end = p + len;

  while (p < end) {
    unsigned char c = *p++;

    if (c < 0x80) {
      while (p < end && *p < 0x80) ++p;
      continue;
    }

    if (c < 0xC2) return false;

    if (c < 0xE0) {
      if (p >= end || (p[0] & 0xC0u) != 0x80u) return false;
      ++p;
      continue;
    }

    if (c < 0xF0) {
      if (end - p < 2) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      if ((b1 & 0xC0u) != 0x80u || (b2 & 0xC0u) != 0x80u) return false;
      if (c == 0xE0) {
        if (b1 < 0xA0) return false;
      } else if (c == 0xED) {
        if (b1 > 0x9F) return false;
      }
      p += 2;
      continue;
    }

    if (c < 0xF5) {
      if (end - p < 3) return false;
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      const unsigned char b3 = p[2];
      if ((b1 & 0xC0u) != 0x80u || (b2 & 0xC0u) != 0x80u || (b3 & 0xC0u) != 0x80u) return false;
      if (c == 0xF0) {
        if (b1 < 0x90) return false;
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

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;

  const char* const base = input.data();
  const char* p = base;
  const char* const end = base + input.size();

  while (p < end) {
    const char* line_start = p;

    while (p < end && *p != '\n') ++p;

    const std::size_t len = static_cast<std::size_t>(p - line_start);
    count += valid_utf8_scalar(line_start, len) ? 1u : 0u;

    if (p < end) ++p;
  }

  return count;
}
