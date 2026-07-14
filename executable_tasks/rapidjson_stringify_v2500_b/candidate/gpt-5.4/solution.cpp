#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void append_uint32(std::string& out, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    buf[--pos] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  out.append(buf + pos, static_cast<std::size_t>(10 - pos));
}

inline void append_hex4(std::string& out, unsigned v) {
  static constexpr char kHex[] = "0123456789abcdef";
  out.push_back(kHex[(v >> 12) & 0xF]);
  out.push_back(kHex[(v >> 8) & 0xF]);
  out.push_back(kHex[(v >> 4) & 0xF]);
  out.push_back(kHex[v & 0xF]);
}

inline void append_escaped_json_string(std::string& out, const std::string& s) {
  out.push_back('"');
  const std::size_t n = s.size();
  std::size_t start = 0;

  for (std::size_t i = 0; i < n; ++i) {
    const unsigned char c = static_cast<unsigned char>(s[i]);
    bool needs_escape = false;
    switch (c) {
      case '"':
      case '\\':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        needs_escape = true;
        break;
      default:
        needs_escape = (c < 0x20);
        break;
    }

    if (!needs_escape) continue;

    if (i > start) out.append(s.data() + start, i - start);

    switch (c) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        out += "\\u";
        append_hex4(out, c);
        break;
    }

    start = i + 1;
  }

  if (start < n) out.append(s.data() + start, n - start);
  out.push_back('"');
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve_size = 2;
  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 32 + names[i].size();
  }

  std::string out;
  out.reserve(reserve_size);
  out.push_back('[');

  for (std::size_t i = 0; i < n; ++i) {
    if (i != 0) out.push_back(',');
    out += "{\"id\":";
    append_uint32(out, ids[i]);
    out += ",\"flag\":";
    out += ((ids[i] % 3U) == 0U) ? "true" : "false";
    out += ",\"value\":";
    append_uint32(out, values[i]);
    out += ",\"name\":";
    append_escaped_json_string(out, names[i]);
    out.push_back('}');
  }

  out.push_back(']');
  return out;
}
