#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline void append_u32(std::string &out, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    buf[--pos] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  out.append(buf + pos, static_cast<std::size_t>(10 - pos));
}

static inline void append_json_escaped(std::string &out, const std::string &s) {
  static constexpr char hex[] = "0123456789abcdef";
  out.push_back('"');

  std::size_t start = 0;
  const std::size_t n = s.size();
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
        out += "\\u00";
        out.push_back(hex[c >> 4]);
        out.push_back(hex[c & 0x0F]);
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
  reserve_size += n * 40;
  for (const auto &name : names) reserve_size += name.size() + 2;

  std::string out;
  out.reserve(reserve_size);
  out.push_back('[');

  for (std::size_t i = 0; i < n; ++i) {
    if (i) out.push_back(',');
    out += "{\"id\":";
    append_u32(out, ids[i]);
    out += ",\"flag\":";
    out += ((ids[i] % 3U) == 0U) ? "true" : "false";
    out += ",\"value\":";
    append_u32(out, values[i]);
    out += ",\"name\":";
    append_json_escaped(out, names[i]);
    out.push_back('}');
  }

  out.push_back(']');
  return out;
}
