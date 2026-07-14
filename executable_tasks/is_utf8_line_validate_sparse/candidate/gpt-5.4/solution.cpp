#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {

static inline bool validate_utf8_scalar(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;

  while (p < end) {
    // Fast ASCII run.
    while (p < end && *p < 0x80) {
      ++p;
    }
    if (p >= end) return true;

    const unsigned char c = *p++;

    if (c < 0xC2) return false;

    if (c < 0xE0) {
      if (p >= end) return false;
      if ((p[0] & 0xC0u) != 0x80u) return false;
      ++p;
      continue;
    }

    if (c < 0xF0) {
      if (static_cast<std::size_t>(end - p) < 2) return false;
      const unsigned char c1 = p[0];
      const unsigned char c2 = p[1];
      if ((c2 & 0xC0u) != 0x80u) return false;
      if (c == 0xE0) {
        if (c1 < 0xA0 || c1 > 0xBF) return false;
      } else if (c == 0xED) {
        if (c1 < 0x80 || c1 > 0x9F) return false;
      } else {
        if ((c1 & 0xC0u) != 0x80u) return false;
      }
      p += 2;
      continue;
    }

    if (c < 0xF5) {
      if (static_cast<std::size_t>(end - p) < 3) return false;
      const unsigned char c1 = p[0];
      const unsigned char c2 = p[1];
      const unsigned char c3 = p[2];
      if ((c2 & 0xC0u) != 0x80u || (c3 & 0xC0u) != 0x80u) return false;
      if (c == 0xF0) {
        if (c1 < 0x90 || c1 > 0xBF) return false;
      } else if (c == 0xF4) {
        if (c1 < 0x80 || c1 > 0x8F) return false;
      } else {
        if ((c1 & 0xC0u) != 0x80u) return false;
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

  const char* base = input.data();
  const char* p = base;
  const char* const end = base + input.size();

  while (p < end) {
    const char* line_start = p;

    while (p < end) {
      const char* nl = static_cast<const char*>(__builtin_memchr(p, '\n', static_cast<std::size_t>(end - p)));
      if (nl == nullptr) {
        p = end;
        break;
      }
      p = nl;
      break;
    }

    const char* line_end = p;
    if (validate_utf8_scalar(line_start, static_cast<std::size_t>(line_end - line_start))) {
      ++count;
    }

    if (p < end) {
      ++p;
    }
  }

  return count;
}
