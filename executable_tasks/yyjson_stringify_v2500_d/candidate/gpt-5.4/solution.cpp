#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  std::size_t total = 2; // '[' and ']'
  for (std::size_t i = 0; i < n; ++i) {
    total += 30; // fixed syntax and keys: {"id":,"flag":,"value":,"name":""}
    total += names[i].size();

    uint32_t x = ids[i];
    total += (x >= 1000000000u) ? 10 :
             (x >= 100000000u) ? 9 :
             (x >= 10000000u) ? 8 :
             (x >= 1000000u) ? 7 :
             (x >= 100000u) ? 6 :
             (x >= 10000u) ? 5 :
             (x >= 1000u) ? 4 :
             (x >= 100u) ? 3 :
             (x >= 10u) ? 2 : 1;

    x = values[i];
    total += (x >= 1000000000u) ? 10 :
             (x >= 100000000u) ? 9 :
             (x >= 10000000u) ? 8 :
             (x >= 1000000u) ? 7 :
             (x >= 100000u) ? 6 :
             (x >= 10000u) ? 5 :
             (x >= 1000u) ? 4 :
             (x >= 100u) ? 3 :
             (x >= 10u) ? 2 : 1;

    total += ((ids[i] % 3u) == 0u) ? 4 : 5; // true / false
  }
  if (n > 0) total += n - 1; // commas between objects

  std::string out;
  out.reserve(total);
  out.push_back('[');

  auto append_u32 = [&out](uint32_t v) {
    char buf[10];
    int pos = 10;
    do {
      buf[--pos] = static_cast<char>('0' + (v % 10u));
      v /= 10u;
    } while (v != 0u);
    out.append(buf + pos, static_cast<std::size_t>(10 - pos));
  };

  for (std::size_t i = 0; i < n; ++i) {
    if (i) out.push_back(',');
    out += "{\"id\":";
    append_u32(ids[i]);
    out += ",\"flag\":";
    if ((ids[i] % 3u) == 0u) {
      out += "true";
    } else {
      out += "false";
    }
    out += ",\"value\":";
    append_u32(values[i]);
    out += ",\"name\":\"";
    out += names[i];
    out += "\"}";
  }

  out.push_back(']');
  return out;
}
