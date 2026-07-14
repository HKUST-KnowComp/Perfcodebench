#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

inline uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline char* u32toa(uint32_t value, char* buffer) {
  if (value == 0) {
    *buffer++ = '0';
    return buffer;
  }
  char* start = buffer;
  while (value) {
    *buffer++ = '0' + (value % 10);
    value /= 10;
  }
  std::reverse(start, buffer);
  return buffer;
}

inline char* u16toa(uint16_t value, char* buffer) {
  if (value == 0) {
    *buffer++ = '0';
    return buffer;
  }
  char* start = buffer;
  while (value) {
    *buffer++ = '0' + (value % 10);
    value /= 10;
  }
  std::reverse(start, buffer);
  return buffer;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const size_t n = ids.size();
  size_t total_size = 0;
  for (size_t i = 0; i < n; ++i) {
    total_size += 32 + messages[i].size(); // upper bound for numbers + separators + newline
  }

  std::string out;
  out.reserve(total_size);

  uint64_t hash = 0;
  char buf[32];

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      char* p = buf;
      p = u32toa(timestamps[i], p);
      *p++ = '|';
      p = u32toa(ids[i], p);
      *p++ = '|';
      p = u16toa(levels[i], p);
      *p++ = '|';
      out.append(buf, p - buf);
      out.append(messages[i]);
      out.push_back('\n');
    }
    hash = checksum_text(out);
  }
  return hash;
}