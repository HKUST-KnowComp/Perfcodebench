#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

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
    uint32_t q = v / 10u;
    buf[--pos] = static_cast<char>('0' + (v - q * 10u));
    v = q;
  } while (v != 0);
  out.append(buf + pos, 10 - pos);
}

inline std::size_t escaped_json_size(const std::string &s) {
  std::size_t extra = 0;
  for (unsigned char c : s) {
    switch (c) {
      case '"':
      case '\\':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        ++extra;
        break;
      default:
        if (c < 0x20) extra += 5;
        break;
    }
  }
  return s.size() + extra;
}

inline void append_json_escaped(std::string &out, const std::string &s) {
  static constexpr char hex[] = "0123456789abcdef";
  const char *p = s.data();
  const char *end = p + s.size();
  const char *chunk = p;

  while (p != end) {
    unsigned char c = static_cast<unsigned char>(*p);
    const char *esc = nullptr;
    char short_esc = 0;
    switch (c) {
      case '"': esc = "\\\""; break;
      case '\\': esc = "\\\\"; break;
      case '\b': short_esc = 'b'; break;
      case '\f': short_esc = 'f'; break;
      case '\n': short_esc = 'n'; break;
      case '\r': short_esc = 'r'; break;
      case '\t': short_esc = 't'; break;
      default: break;
    }

    if (esc || short_esc || c < 0x20) {
      if (p != chunk) out.append(chunk, static_cast<std::size_t>(p - chunk));
      if (esc) {
        out.append(esc, 2);
      } else if (short_esc) {
        out.push_back('\\');
        out.push_back(short_esc);
      } else {
        char buf[6];
        buf[0] = '\\';
        buf[1] = 'u';
        buf[2] = '0';
        buf[3] = '0';
        buf[4] = hex[c >> 4];
        buf[5] = hex[c & 0x0F];
        out.append(buf, 6);
      }
      ++p;
      chunk = p;
    } else {
      ++p;
    }
  }

  if (chunk != end) out.append(chunk, static_cast<std::size_t>(end - chunk));
}

}  // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t total = 2;  // [ ]
  for (std::size_t i = 0; i < n; ++i) {
    total += 35;  // punctuation + keys + bool text + quotes around name/object
    total += count_digits_u32(ids[i]);
    total += count_digits_u32(values[i]);
    total += escaped_json_size(names[i]);
  }
  if (n > 1) total += (n - 1);

  std::string out;
  out.reserve(total);
  out.push_back('[');

  for (std::size_t i = 0; i < n; ++i) {
    if (i) out.push_back(',');
    out.append("{\"id\":", 6);
    append_u32(out, ids[i]);
    out.append(",\"flag\":", 9);
    if ((ids[i] % 3u) == 0u) {
      out.append("true", 4);
    } else {
      out.append("false", 5);
    }
    out.append(",\"value\":", 10);
    append_u32(out, values[i]);
    out.append(",\"name\":\"", 10);
    append_json_escaped(out, names[i]);
    out.append("\"}", 2);
  }

  out.push_back(']');
  return out;
}
