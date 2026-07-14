#include "interface.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cstddef>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

template<typename T>
void append_int(std::string& s, T val) {
  if (val == 0) {
    s += '0';
    return;
  }
  char buf[20];
  char* p = buf;
  while (val > 0) {
    *p++ = static_cast<char>((val % 10) + '0');
    val /= 10;
  }
  std::reverse(buf, p);
  s.append(buf, static_cast<size_t>(p - buf));
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  std::string out;
  uint64_t hash = 0;
  const size_t n = ids.size();

  size_t total_capacity = 0;
  for (size_t i = 0; i < n; ++i) {
    total_capacity += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
  }
  out.reserve(total_capacity);

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      append_int(out, timestamps[i]);
      out += '|';
      append_int(out, ids[i]);
      out += '|';
      append_int(out, levels[i]);
      out += '|';
      out += messages[i];
      out += '\n';
    }
    hash = checksum_text(out);
  }
  return hash;
}