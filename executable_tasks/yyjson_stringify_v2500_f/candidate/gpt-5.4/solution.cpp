#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

static inline uint32_t count_digits_u32(uint32_t v) {
  if (v >= 1000000000u) return 10;
  if (v >= 100000000u) return 9;
  if (v >= 10000000u) return 8;
  if (v >= 1000000u) return 7;
  if (v >= 100000u) return 6;
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static inline char* write_u32(char* dst, uint32_t v) {
  uint32_t digits = count_digits_u32(v);
  char* p = dst + digits;
  do {
    *--p = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  return dst + digits;
}

static inline std::size_t escaped_json_string_size(const std::string& s) {
  std::size_t n = 2; // quotes
  for (unsigned char c : s) {
    switch (c) {
      case '"':
      case '\\':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        n += 2;
        break;
      default:
        if (c < 0x20u) {
          n += 6;
        } else {
          n += 1;
        }
        break;
    }
  }
  return n;
}

static inline char hex_digit(unsigned v) {
  return static_cast<char>(v < 10 ? ('0' + v) : ('a' + (v - 10)));
}

static inline char* write_escaped_json_string(char* dst, const std::string& s) {
  *dst++ = '"';
  for (unsigned char c : s) {
    switch (c) {
      case '"':
        *dst++ = '\\';
        *dst++ = '"';
        break;
      case '\\':
        *dst++ = '\\';
        *dst++ = '\\';
        break;
      case '\b':
        *dst++ = '\\';
        *dst++ = 'b';
        break;
      case '\f':
        *dst++ = '\\';
        *dst++ = 'f';
        break;
      case '\n':
        *dst++ = '\\';
        *dst++ = 'n';
        break;
      case '\r':
        *dst++ = '\\';
        *dst++ = 'r';
        break;
      case '\t':
        *dst++ = '\\';
        *dst++ = 't';
        break;
      default:
        if (c < 0x20u) {
          *dst++ = '\\';
          *dst++ = 'u';
          *dst++ = '0';
          *dst++ = '0';
          *dst++ = hex_digit(c >> 4);
          *dst++ = hex_digit(c & 0x0Fu);
        } else {
          *dst++ = static_cast<char>(c);
        }
        break;
    }
  }
  *dst++ = '"';
  return dst;
}

} // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t total = 2; // [ ]
  if (n != 0) total += (n - 1); // commas between objects

  for (std::size_t i = 0; i < n; ++i) {
    total += 30; // {"id":,"flag":,"value":,"name":}
    total += count_digits_u32(ids[i]);
    total += ((ids[i] % 3u) == 0u) ? 4 : 5; // true / false
    total += count_digits_u32(values[i]);
    total += escaped_json_string_size(names[i]);
  }

  std::string out;
  out.resize(total);
  char* p = out.data();

  *p++ = '[';
  for (std::size_t i = 0; i < n; ++i) {
    if (i) *p++ = ',';

    *p++ = '{';

    std::memcpy(p, "\"id\":", 5);
    p += 5;
    p = write_u32(p, ids[i]);

    std::memcpy(p, ",\"flag\":", 9);
    p += 9;
    if ((ids[i] % 3u) == 0u) {
      std::memcpy(p, "true", 4);
      p += 4;
    } else {
      std::memcpy(p, "false", 5);
      p += 5;
    }

    std::memcpy(p, ",\"value\":", 10);
    p += 10;
    p = write_u32(p, values[i]);

    std::memcpy(p, ",\"name\":", 9);
    p += 9;
    p = write_escaped_json_string(p, names[i]);

    *p++ = '}';
  }
  *p++ = ']';

  return out;
}
