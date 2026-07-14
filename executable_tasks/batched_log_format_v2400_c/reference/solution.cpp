#include "interface.h"

#include <string>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void append_u32(std::string& out, uint32_t v) {
  char buf[16];
  int n = 0;
  do {
    buf[n++] = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  while (n--) {
    out.push_back(buf[n]);
  }
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
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    std::size_t total = 0;
    for (std::size_t i = 0; i < ids.size(); ++i) {
      total += messages[i].size() + 32;
    }
    out.reserve(total);
    for (std::size_t i = 0; i < ids.size(); ++i) {
      append_u32(out, timestamps[i]);
      out.push_back('|');
      append_u32(out, ids[i]);
      out.push_back('|');
      append_u32(out, levels[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
    }
    hash = checksum_text(out);
  }
  return hash;
}
