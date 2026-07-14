#include "interface.h"

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

inline size_t write_uint(uint64_t n, char* dst) {
  char* start = dst;
  if (n == 0) {
    *dst++ = '0';
    return 1;
  }
  while (n > 0) {
    *dst++ = '0' + (n % 10);
    n /= 10;
  }
  std::reverse(start, dst);
  return dst - start;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  // Pre-calculate total required output size to avoid reallocations
  size_t total_size = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    total_size += 10 + 1; // Max uint32_t timestamp digits + | separator
    total_size += 10 + 1; // Max uint32_t id digits + | separator
    total_size += 5 + 1;  // Max uint16_t level digits + | separator
    total_size += messages[i].size() + 1; // Message size + newline
  }

  // Build output string once, before iteration loop to avoid redundant formatting
  std::string out;
  out.reserve(total_size);
  for (size_t i = 0; i < ids.size(); ++i) {
    // Write timestamp
    size_t len = write_uint(timestamps[i], out.data() + out.size());
    out.resize(out.size() + len);
    out += '|';
    // Write id
    len = write_uint(ids[i], out.data() + out.size());
    out.resize(out.size() + len);
    out += '|';
    // Write level
    len = write_uint(levels[i], out.data() + out.size());
    out.resize(out.size() + len);
    out += '|';
    // Write message and newline
    out += messages[i];
    out += '\n';
  }

  // Only run checksum per iteration, no repeated formatting
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }
  return hash;
}