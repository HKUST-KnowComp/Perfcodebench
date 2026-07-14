#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : text) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void write_uint32(std::string& out, uint32_t val) {
  char buf[12];
  int len = 0;
  do {
    buf[len++] = '0' + (val % 10);
    val /= 10;
  } while (val);
  for (int i = 0; i < len / 2; ++i) {
    std::swap(buf[i], buf[len - 1 - i]);
  }
  out.append(buf, len);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0;

  std::string out;
  // Estimate size to avoid reallocations (approx 10 digits + separators + message)
  out.reserve(ids.size() * 30);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    write_uint32(out, timestamps[i]);
    out.push_back('|');
    write_uint32(out, ids[i]);
    out.push_back('|');
    write_uint32(out, levels[i]);
    out.push_back('|');
    out.append(messages[i]);
    out.push_back('\n');
  }

  return checksum_text(out);
}