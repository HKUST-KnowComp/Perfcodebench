#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void append_uint32(std::string& s, uint32_t val) {
  if (val == 0) {
    s.push_back('0');
    return;
  }
  char buf[10];
  int pos = 10;
  while (val > 0) {
    buf[--pos] = '0' + (val % 10);
    val /= 10;
  }
  s.append(buf + pos, 10 - pos);
}

inline void append_uint16(std::string& s, uint16_t val) {
  if (val == 0) {
    s.push_back('0');
    return;
  }
  char buf[5];
  int pos = 5;
  while (val > 0) {
    buf[--pos] = '0' + (val % 10);
    val /= 10;
  }
  s.append(buf + pos, 5 - pos);
}

uint64_t checksum_text_inline(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* data = reinterpret_cast<const unsigned char*>(text.data());
  size_t len = text.size();
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  std::string out;
  size_t n = ids.size();
  
  // Estimate capacity: rough estimate per line
  size_t estimated_size = 0;
  for (size_t i = 0; i < n; ++i) {
    estimated_size += 30 + messages[i].size();
  }
  out.reserve(estimated_size);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      append_uint32(out, timestamps[i]);
      out.push_back('|');
      append_uint32(out, ids[i]);
      out.push_back('|');
      append_uint16(out, levels[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
    }
    hash = checksum_text_inline(out);
  }
  return hash;
}
