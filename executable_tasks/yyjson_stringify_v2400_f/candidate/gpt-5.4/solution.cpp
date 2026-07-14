#include "interface.h"

#include <yyjson.h>

#include <charconv>
#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Precompute output size to avoid reallocations.
  // Each object is: {"id":<id>,"flag":<bool>,"value":<value>,"name":"<escaped>"}
  // Field order must be preserved exactly.
  std::size_t total = 2;  // '[' and ']'
  if (n > 1) total += (n - 1);  // commas between objects

  for (std::size_t i = 0; i < n; ++i) {
    total += 31;  // punctuation + keys + quotes around name, excluding values and escaped name expansion

    uint32_t id = ids[i];
    uint32_t value = values[i];

    auto digits10_u32 = [](uint32_t x) -> std::size_t {
      if (x >= 1000000000u) return 10;
      if (x >= 100000000u) return 9;
      if (x >= 10000000u) return 8;
      if (x >= 1000000u) return 7;
      if (x >= 100000u) return 6;
      if (x >= 10000u) return 5;
      if (x >= 1000u) return 4;
      if (x >= 100u) return 3;
      if (x >= 10u) return 2;
      return 1;
    };

    total += digits10_u32(id);
    total += ((id % 3u) == 0u) ? 4 : 5;  // true / false
    total += digits10_u32(value);

    const std::string& s = names[i];
    for (unsigned char c : s) {
      switch (c) {
        case '"':
        case '\\':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
          total += 2;
          break;
        default:
          if (c < 0x20) {
            total += 6;  // \u00XX
          } else {
            total += 1;
          }
          break;
      }
    }
  }

  std::string out;
  out.resize(total);
  char* p = out.data();

  auto append_lit = [&p](const char* s, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) p[i] = s[i];
    p += len;
  };

  auto append_u32 = [&p](uint32_t v) {
    auto res = std::to_chars(p, p + 10, v);
    p = res.ptr;
  };

  auto append_escaped_json = [&p](const std::string& s) {
    static constexpr char hex[] = "0123456789abcdef";
    for (unsigned char c : s) {
      switch (c) {
        case '"':
          *p++ = '\\';
          *p++ = '"';
          break;
        case '\\':
          *p++ = '\\';
          *p++ = '\\';
          break;
        case '\b':
          *p++ = '\\';
          *p++ = 'b';
          break;
        case '\f':
          *p++ = '\\';
          *p++ = 'f';
          break;
        case '\n':
          *p++ = '\\';
          *p++ = 'n';
          break;
        case '\r':
          *p++ = '\\';
          *p++ = 'r';
          break;
        case '\t':
          *p++ = '\\';
          *p++ = 't';
          break;
        default:
          if (c < 0x20) {
            *p++ = '\\';
            *p++ = 'u';
            *p++ = '0';
            *p++ = '0';
            *p++ = hex[c >> 4];
            *p++ = hex[c & 0x0F];
          } else {
            *p++ = static_cast<char>(c);
          }
          break;
      }
    }
  };

  *p++ = '[';
  for (std::size_t i = 0; i < n; ++i) {
    if (i) *p++ = ',';
    *p++ = '{';

    append_lit("\"id\":", 5);
    const uint32_t id = ids[i];
    append_u32(id);

    append_lit(",\"flag\":", 8);
    if ((id % 3u) == 0u) {
      append_lit("true", 4);
    } else {
      append_lit("false", 5);
    }

    append_lit(",\"value\":", 10);
    append_u32(values[i]);

    append_lit(",\"name\":\"", 10);
    append_escaped_json(names[i]);
    *p++ = '"';
    *p++ = '}';
  }
  *p++ = ']';

  out.resize(static_cast<std::size_t>(p - out.data()));
  return out;
}
