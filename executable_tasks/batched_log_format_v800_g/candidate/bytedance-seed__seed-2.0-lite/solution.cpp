#include "interface.h"

#include <string>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

char* append_uint(char* buf, uint64_t v) {
  if (v == 0) {
    *buf++ = '0';
    return buf;
  }
  char tmp[20];
  char* p = tmp + 19;
  while (v > 0) {
    *p-- = (v % 10) + '0';
    v /= 10;
  }
  size_t len = tmp + 19 - p;
  std::memcpy(buf, p + 1, len);
  return buf + len;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0 || ids.empty()) return 0;

  size_t total_size = 0;
  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    total_size += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
  }

  std::string out;
  out.reserve(total_size);
  for (size_t i = 0; i < n; ++i) {
    char* buf = out.data() + out.size();
    char* end = append_uint(buf, timestamps[i]);
    *end++ = '|';
    end = append_uint(end, ids[i]);
    *end++ = '|';
    end = append_uint(end, levels[i]);
    *end++ = '|';
    out.resize(end - out.data());
    out.append(messages[i]);
    out += '\n';
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }
  return hash;
}