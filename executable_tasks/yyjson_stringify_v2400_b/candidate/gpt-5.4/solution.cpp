#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Fast path for empty input.
  if (n == 0) return "[]";

  // Conservative pre-reservation to minimize reallocations.
  // Per object fixed syntax is about 39 chars plus digits and name bytes:
  // {"id":,"flag":,"value":,"name":""}
  std::size_t reserve_size = 2;  // [ ]
  reserve_size += n > 0 ? (n - 1) : 0;  // commas between objects
  for (std::size_t i = 0; i < n; ++i) {
    reserve_size += 39;
    reserve_size += names[i].size();
    reserve_size += 10;  // id digits max for uint32_t
    reserve_size += 10;  // value digits max for uint32_t
    // Extra room for escaping in names.
    reserve_size += names[i].size() / 8 + 8;
  }

  std::string out;
  out.reserve(reserve_size);

  auto append_u32 = [&](uint32_t v) {
    char buf[10];
    int pos = 10;
    do {
      buf[--pos] = static_cast<char>('0' + (v % 10));
      v /= 10;
    } while (v != 0);
    out.append(buf + pos, static_cast<std::size_t>(10 - pos));
  };

  auto append_escaped_json_string = [&](const std::string& s) {
    out.push_back('"');
    std::size_t start = 0;
    const std::size_t len = s.size();
    for (std::size_t i = 0; i < len; ++i) {
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
        default: {
          static constexpr char hex[] = "0123456789abcdef";
          char esc[6] = {'\\', 'u', '0', '0', hex[c >> 4], hex[c & 0x0F]};
          out.append(esc, 6);
          break;
        }
      }
      start = i + 1;
    }
    if (start < len) out.append(s.data() + start, len - start);
    out.push_back('"');
  };

  out.push_back('[');
  for (std::size_t i = 0; i < n; ++i) {
    if (i != 0) out.push_back(',');
    out += "{\"id\":";
    append_u32(ids[i]);
    out += ",\"flag\":";
    out += ((ids[i] % 3U) == 0U) ? "true" : "false";
    out += ",\"value\":";
    append_u32(values[i]);
    out += ",\"name\":";
    append_escaped_json_string(names[i]);
    out.push_back('}');
  }
  out.push_back(']');

  return out;
}
