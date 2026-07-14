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

void append_uint32(std::string& out, uint32_t num) {
  if (num == 0) {
    out += '0';
    return;
  }
  char buffer[12];
  int idx = 0;
  while (num > 0) {
    buffer[idx++] = '0' + (num % 10);
    num /= 10;
  }
  for (int i = idx - 1; i >= 0; --i) {
    out += buffer[i];
  }
}

void append_uint16(std::string& out, uint16_t num) {
  if (num == 0) {
    out += '0';
    return;
  }
  char buffer[6];
  int idx = 0;
  while (num > 0) {
    buffer[idx++] = '0' + (num % 10);
    num /= 10;
  }
  for (int i = idx - 1; i >= 0; --i) {
    out += buffer[i];
  }
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
  out.reserve(timestamps.size() * 100);

  for (std::size_t i = 0; i < timestamps.size(); ++i) {
    append_uint32(out, timestamps[i]);
    out += '|';
    append_uint32(out, ids[i]);
    out += '|';
    append_uint16(out, levels[i]);
    out += '|';
    out += messages[i];
    out += '\n';
  }

  return checksum_text(out);
}