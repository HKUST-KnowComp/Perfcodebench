#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void append_uint32(std::string &out, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    buf[--pos] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  out.append(buf + pos, static_cast<std::size_t>(10 - pos));
}

inline void append_json_escaped_string(std::string &out, const std::string &s) {
  out.push_back('"');
  const char *data = s.data();
  const std::size_t len = s.size();
  std::size_t start = 0;

  for (std::size_t i = 0; i < len; ++i) {
    const unsigned char c = static_cast<unsigned char>(data[i]);
    const bool needs_escape =
        (c == '"') || (c == '\\') || (c <= 0x1F);
    if (!needs_escape) continue;

    if (i > start) out.append(data + start, i - start);

    switch (c) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: {
        static constexpr char hex[] = "0123456789abcdef";
        char esc[6] = {'\\', 'u', '0', '0', hex[c >> 4], hex[c & 0x0F]};
        out.append(esc, 6);
        break;
      }
    }
    start = i + 1;
  }

  if (start < len) out.append(data + start, len - start);
  out.push_back('"');
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t reserve = 2;
  for (std::size_t i = 0; i < n; ++i) {
    reserve += 32 + names[i].size();
  }

  std::string out;
  out.reserve(reserve);
  out.push_back('[');

  for (std::size_t i = 0; i < n; ++i) {
    if (i) out.push_back(',');
    out += "{\"id\":";
    append_uint32(out, ids[i]);
    out += ",\"flag\":";
    out += ((ids[i] % 3U) == 0U) ? "true" : "false";
    out += ",\"value\":";
    append_uint32(out, values[i]);
    out += ",\"name\":";
    append_json_escaped_string(out, names[i]);
    out.push_back('}');
  }

  out.push_back(']');
  return out;
}
