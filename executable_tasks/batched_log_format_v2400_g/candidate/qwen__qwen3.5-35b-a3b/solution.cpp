#pragma once

#include <cstdint>
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

void format_uint32(uint32_t value, char* buffer) {
  char* p = buffer + 10;
  *--p = '\0';
  do {
    *--p = '0' + (value % 10);
    value /= 10;
  } while (value > 0);
}

void format_uint16(uint16_t value, char* buffer) {
  char* p = buffer + 5;
  *--p = '\0';
  do {
    *--p = '0' + (value % 10);
    value /= 10;
  } while (value > 0);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (timestamps.empty()) return 0;

  std::string out;
  out.reserve(timestamps.size() * 30 + 1000); // Heuristic reservation

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (std::size_t i = 0; i < timestamps.size(); ++i) {
      char num_buf[16];
      char* p = num_buf;

      format_uint32(timestamps[i], p);
      out.append(p);
      out.push_back('|');

      format_uint32(ids[i], p);
      out.append(p);
      out.push_back('|');

      format_uint16(levels[i], p);
      out.append(p);
      out.push_back('|');

      out.append(messages[i]);
      out.push_back('\n');
    }
  }

  return checksum_text(out);
}