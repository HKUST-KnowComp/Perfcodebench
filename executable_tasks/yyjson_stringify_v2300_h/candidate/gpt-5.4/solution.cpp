#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t total = 2;  // '[' and ']'
  if (n > 0) total += (n - 1);  // commas between objects

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t id = ids[i];
    const uint32_t value = values[i];
    const std::string& name = names[i];

    total += 31;  // {"id":,"flag":,"value":,"name":""}

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

    for (unsigned char c : name) {
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
          total += (c < 0x20) ? 6 : 1;
          break;
      }
    }
  }

  std::string out;
  out.resize(total);
  char* p = out.data();

  auto append_lit = [&p](const char* s) {
    while (*s) *p++ = *s++;
  };

  auto append_u32 = [&p](uint32_t x) {
    char buf[10];
    int len = 0;
    do {
      buf[len++] = static_cast<char>('0' + (x % 10u));
      x /= 10u;
    } while (x != 0u);
    while (len--) *p++ = buf[len];
  };

  auto append_hex4 = [&p](unsigned char c) {
    static constexpr char hex[] = "0123456789abcdef";
    *p++ = '\\';
    *p++ = 'u';
    *p++ = '0';
    *p++ = '0';
    *p++ = hex[c >> 4];
    *p++ = hex[c & 0x0F];
  };

  auto append_escaped_json_string = [&p, &append_hex4](const std::string& s) {
    *p++ = '"';
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
            append_hex4(c);
          } else {
            *p++ = static_cast<char>(c);
          }
          break;
      }
    }
    *p++ = '"';
  };

  *p++ = '[';
  for (std::size_t i = 0; i < n; ++i) {
    if (i) *p++ = ',';

    const uint32_t id = ids[i];

    *p++ = '{';
    append_lit("\"id\":");
    append_u32(id);
    append_lit(",\"flag\":");
    if ((id % 3u) == 0u) {
      append_lit("true");
    } else {
      append_lit("false");
    }
    append_lit(",\"value\":");
    append_u32(values[i]);
    append_lit(",\"name\":");
    append_escaped_json_string(names[i]);
    *p++ = '}';
  }
  *p++ = ']';

  return out;
}
