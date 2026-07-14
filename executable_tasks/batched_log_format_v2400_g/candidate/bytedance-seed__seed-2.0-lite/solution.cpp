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
  std::vector<std::string> ts_strs, id_strs, lvl_strs;
  ts_strs.reserve(n);
  id_strs.reserve(n);
  lvl_strs.reserve(n);
  size_t total_buffer_size = 0;

  // Precompute all fixed numeric strings and calculate total required buffer size once
  for (size_t i = 0; i < n; ++i) {
    ts_strs.emplace_back(std::to_string(timestamps[i]));
    id_strs.emplace_back(std::to_string(ids[i]));
    lvl_strs.emplace_back(std::to_string(levels[i]));
    total_buffer_size += ts_strs.back().size() + id_strs.back().size() + lvl_strs.back().size() + messages[i].size() + 4;
  }

  std::string out;
  out.reserve(total_buffer_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      out.append(ts_strs[i]);
      out.append(1, '|');
      out.append(id_strs[i]);
      out.append(1, '|');
      out.append(lvl_strs[i]);
      out.append(1, '|');
      out.append(messages[i]);
      out.append(1, '\n');
    }
    hash = checksum_text(out);
  }
  return hash;
}