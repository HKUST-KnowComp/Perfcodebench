#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) return "[]";

  std::size_t total = 2; // [ ]

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t id = ids[i];
    const uint32_t value = values[i];
    const std::size_t name_len = names[i].size();

    std::size_t id_digits = 1;
    if (id >= 10U) {
      if (id >= 100U) {
        if (id >= 1000U) {
          if (id >= 10000U) {
            if (id >= 100000U) {
              if (id >= 1000000U) {
                if (id >= 10000000U) {
                  if (id >= 100000000U) {
                    if (id >= 1000000000U) id_digits = 10;
                    else id_digits = 9;
                  } else {
                    id_digits = 8;
                  }
                } else {
                  id_digits = 7;
                }
              } else {
                id_digits = 6;
              }
            } else {
              id_digits = 5;
            }
          } else {
            id_digits = 4;
          }
        } else {
          id_digits = 3;
        }
      } else {
        id_digits = 2;
      }
    }

    std::size_t value_digits = 1;
    if (value >= 10U) {
      if (value >= 100U) {
        if (value >= 1000U) {
          if (value >= 10000U) {
            if (value >= 100000U) {
              if (value >= 1000000U) {
                if (value >= 10000000U) {
                  if (value >= 100000000U) {
                    if (value >= 1000000000U) value_digits = 10;
                    else value_digits = 9;
                  } else {
                    value_digits = 8;
                  }
                } else {
                  value_digits = 7;
                }
              } else {
                value_digits = 6;
              }
            } else {
              value_digits = 5;
            }
          } else {
            value_digits = 4;
          }
        } else {
          value_digits = 3;
        }
      } else {
        value_digits = 2;
      }
    }

    std::size_t escaped_name_len = 0;
    for (unsigned char c : names[i]) {
      switch (c) {
        case '"':
        case '\\':
          escaped_name_len += 2;
          break;
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
          escaped_name_len += 2;
          break;
        default:
          if (c < 0x20) escaped_name_len += 6;
          else escaped_name_len += 1;
          break;
      }
    }

    total += 33 + id_digits + (((id % 3U) == 0U) ? 4 : 5) + value_digits + escaped_name_len;
    if (i + 1 != n) total += 1;
    (void)name_len;
  }

  std::string out;
  out.resize(total);
  char* p = out.data();

  auto append_lit = [&](const char* s) {
    while (*s) *p++ = *s++;
  };

  auto append_u32 = [&](uint32_t v) {
    char buf[10];
    int len = 0;
    do {
      buf[len++] = static_cast<char>('0' + (v % 10U));
      v /= 10U;
    } while (v != 0U);
    while (len--) *p++ = buf[len];
  };

  auto append_escaped_json_string = [&](const std::string& s) {
    *p++ = '"';
    static constexpr char hex[] = "0123456789abcdef";
    for (unsigned char c : s) {
      switch (c) {
        case '"': *p++ = '\\'; *p++ = '"'; break;
        case '\\': *p++ = '\\'; *p++ = '\\'; break;
        case '\b': *p++ = '\\'; *p++ = 'b'; break;
        case '\f': *p++ = '\\'; *p++ = 'f'; break;
        case '\n': *p++ = '\\'; *p++ = 'n'; break;
        case '\r': *p++ = '\\'; *p++ = 'r'; break;
        case '\t': *p++ = '\\'; *p++ = 't'; break;
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
    *p++ = '"';
  };

  *p++ = '[';
  for (std::size_t i = 0; i < n; ++i) {
    if (i) *p++ = ',';
    append_lit("{\"id\":");
    append_u32(ids[i]);
    append_lit(",\"flag\":");
    if ((ids[i] % 3U) == 0U) append_lit("true");
    else append_lit("false");
    append_lit(",\"value\":");
    append_u32(values[i]);
    append_lit(",\"name\":");
    append_escaped_json_string(names[i]);
    *p++ = '}';
  }
  *p++ = ']';

  return out;
}
