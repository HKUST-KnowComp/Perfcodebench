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

constexpr int count_digits(uint64_t n) {
  if (n == 0) return 1;
  int cnt = 0;
  while (n > 0) {
    cnt++;
    n /= 10;
  }
  return cnt;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  std::string full_log;
  const size_t n = ids.size();
  if (n > 0) {
    size_t total_size = 0;
    for (size_t i = 0; i < n; i++) {
      total_size += count_digits(timestamps[i]);
      total_size += count_digits(ids[i]);
      total_size += count_digits(levels[i]);
      total_size += messages[i].size();
      total_size += 4;
    }
    full_log.reserve(total_size);
    for (size_t i = 0; i < n; i++) {
      full_log += std::to_string(timestamps[i]);
      full_log += '|';
      full_log += std::to_string(ids[i]);
      full_log += '|';
      full_log += std::to_string(levels[i]);
      full_log += '|';
      full_log += messages[i];
      full_log += '\n';
    }
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(full_log);
  }
  return hash;
}