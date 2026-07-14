#include "interface.h"

#include <string>
#include <vector>

namespace {

inline uint32_t count_digits_u32(uint32_t v) {
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

inline void append_u32(std::string &out, uint32_t v) {
  char buf[10];
  uint32_t pos = 10;
  do {
    buf[--pos] = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  out.append(buf + pos, 10 - pos);
}

inline void append_json_escaped_string(std::string &out, const std::string &s) {
  out.push_back('"');
  const size_t n = s.size();
  size_t i = 0;
  while (i < n) {
    size_t start = i;
    while (i < n) {
      unsigned char c = static_cast<unsigned char>(s[i]);
      if (c == '"' || c == '\\' || c < 0x20) break;
      ++i;
    }
    if (i > start) out.append(s.data() + start, i - start);
    if (i == n) break;

    unsigned char c = static_cast<unsigned char>(s[i++]);
    switch (c) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: {
        static const char hex[] = "0123456789abcdef";
        out += "\\u00";
        out.push_back(hex[c >> 4]);
        out.push_back(hex[c & 0x0F]);
        break;
      }
    }
  }
  out.push_back('"');
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();

  size_t total = 2;  // [ ]
  if (n != 0) total += n - 1;  // commas between objects

  for (size_t i = 0; i < n; ++i) {
    total += 30;  // punctuation + keys + boolean text excluding numbers and name content
    total += count_digits_u32(ids[i]);
    total += count_digits_u32(values[i]);
    total += names[i].size() + 2;  // quotes around name, escapes may grow further
  }

  std::string out;
  out.reserve(total);
  out.push_back('[');

  for (size_t i = 0; i < n; ++i) {
    if (i) out.push_back(',');
    out += "{\"id\":";
    append_u32(out, ids[i]);
    out += ",\"flag\":";
    out += ((ids[i] % 3u) == 0u) ? "true" : "false";
    out += ",\"value\":";
    append_u32(out, values[i]);
    out += ",\"name\":";
    append_json_escaped_string(out, names[i]);
    out.push_back('}');
  }

  out.push_back(']');
  return out;
}
