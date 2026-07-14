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

inline size_t count_digits(uint32_t n) {
  if (n < 10) return 1;
  if (n < 100) return 2;
  if (n < 1000) return 3;
  if (n < 10000) return 4;
  if (n < 100000) return 5;
  if (n < 1000000) return 6;
  if (n < 10000000) return 7;
  if (n < 100000000) return 8;
  if (n < 1000000000) return 9;
  return 10;
}

inline size_t count_digits(uint16_t n) {
  if (n < 10) return 1;
  if (n < 100) return 2;
  return 3;
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
    total_size += count_digits(timestamps[i]) + 1;
    total_size += count_digits(ids[i]) + 1;
    total_size += count_digits(levels[i]) + 1;
    total_size += messages[i].size() + 1;
  }

  std::string out;
  out.reserve(total_size);
  for (size_t i = 0; i < n; ++i) {
    out += std::to_string(timestamps[i]);
    out += '|';
    out += std::to_string(ids[i]);
    out += '|';
    out += std::to_string(levels[i]);
    out += '|';
    out += messages[i];
    out += '\n';
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }
  return hash;
}