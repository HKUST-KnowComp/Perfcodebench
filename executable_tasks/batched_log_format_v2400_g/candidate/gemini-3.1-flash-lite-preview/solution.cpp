#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>

namespace {

inline uint64_t checksum_text(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
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
  size_t n = ids.size();
  std::vector<std::string> formatted_lines(n);
  size_t total_size = 0;

  for (size_t i = 0; i < n; ++i) {
    formatted_lines[i] = std::to_string(timestamps[i]) + "|" + 
                         std::to_string(ids[i]) + "|" + 
                         std::to_string(levels[i]) + "|" + 
                         messages[i] + "\n";
    total_size += formatted_lines[i].size();
  }

  std::string out;
  out.resize(total_size);
  char* ptr = &out[0];
  for (const auto& line : formatted_lines) {
    std::memcpy(ptr, line.data(), line.size());
    ptr += line.size();
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out.data(), out.size());
  }
  return hash;
}