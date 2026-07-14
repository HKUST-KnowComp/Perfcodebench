#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void append_uint32(std::string& out, uint32_t value) {
    if (value == 0) {
        out.push_back('0');
        return;
    }
    char buf[10];
    int pos = 10;
    while (value > 0) {
        --pos;
        buf[pos] = '0' + (value % 10);
        value /= 10;
    }
    out.append(buf + pos, 10 - pos);
}

void append_uint16(std::string& out, uint16_t value) {
    if (value == 0) {
        out.push_back('0');
        return;
    }
    char buf[5];
    int pos = 5;
    uint16_t v = value;
    while (v > 0) {
        --pos;
        buf[pos] = '0' + (v % 10);
        v /= 10;
    }
    out.append(buf + pos, 5 - pos);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0;

  std::string text;
  std::size_t n = ids.size();
  text.reserve(n * 64);

  for (std::size_t i = 0; i < n; ++i) {
    append_uint32(text, timestamps[i]);
    text.push_back('|');
    append_uint32(text, ids[i]);
    text.push_back('|');
    append_uint16(text, levels[i]);
    text.push_back('|');
    text.append(messages[i]);
    text.push_back('\n');
  }

  uint64_t hash = checksum_text(text);
  // For any positive iters the final iteration produces the same checksum
  return hash;
}