#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline std::size_t count_digits_u32(uint32_t v) {
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

inline void append_u32(std::string& out, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    buf[--pos] = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  out.append(buf + pos, static_cast<std::size_t>(10 - pos));
}

inline void append_json_escaped_string(std::string& out, const std::string& s) {
  out.push_back('"');
  std::size_t start = 0;
  const std::size_t n = s.size();
  for (std::size_t i = 0; i < n; ++i) {
    const unsigned char c = static_cast<unsigned char>(s[i]);
    const char* esc = nullptr;
    switch (c) {
      case '"': esc = "\\\""; break;
      case '\\': esc = "\\\\"; break;
      case '\b': esc = "\\b"; break;
      case '\f': esc = "\\f"; break;
      case '\n': esc = "\\n"; break;
      case '\r': esc = "\\r"; break;
      case '\t': esc = "\\t"; break;
      default:
        if (c < 0x20u) {
          if (i > start) out.append(s.data() + start, i - start);
          out += "\\u00";
          static constexpr char hex[] = "0123456789abcdef";
          out.push_back(hex[c >> 4]);
          out.push_back(hex[c & 0x0Fu]);
          start = i + 1;
        }
        continue;
    }
    if (i > start) out.append(s.data() + start, i - start);
    out += esc;
    start = i + 1;
  }
  if (n > start) out.append(s.data() + start, n - start);
  out.push_back('"');
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve_size = 2;  // [ ]
  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 36;  // punctuation, keys, booleans, quotes around name
    reserve_size += count_digits_u32(ids[i]);
    reserve_size += count_digits_u32(values[i]);
    reserve_size += names[i].size();
  }
  if (n > 1) reserve_size += (n - 1);  // commas between objects

  std::string out;
  out.reserve(reserve_size);

  out.push_back('[');
  for (std::size_t i = 0; i < n; ++i) {
    if (i != 0) out.push_back(',');
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
