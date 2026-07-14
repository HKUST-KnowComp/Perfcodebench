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

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const size_t n = ids.size();
  std::vector<std::string> ts_str(n), id_str(n), level_str(n);
  size_t total_size = 0;
  
  // Precompute all fixed string representations and total output size once
  for (size_t i = 0; i < n; ++i) {
    ts_str[i] = std::to_string(timestamps[i]);
    id_str[i] = std::to_string(ids[i]);
    level_str[i] = std::to_string(levels[i]);
    total_size += ts_str[i].size() + 1 + id_str[i].size() + 1 + level_str[i].size() + 1 + messages[i].size() + 1;
  }
  
  std::string out;
  out.reserve(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      out += ts_str[i];
      out += '|';
      out += id_str[i];
      out += '|';
      out += level_str[i];
      out += '|';
      out += messages[i];
      out += '\n';
    }
    hash = checksum_text(out);
  }
  return hash;
}