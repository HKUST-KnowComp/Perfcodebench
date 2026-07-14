#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

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
  // Precompute string representations for timestamps, ids, levels
  std::vector<std::string> ts_strs;
  ts_strs.reserve(timestamps.size());
  for (uint32_t t : timestamps) {
    ts_strs.push_back(std::to_string(t));
  }

  std::vector<std::string> id_strs;
  id_strs.reserve(ids.size());
  for (uint32_t id : ids) {
    id_strs.push_back(std::to_string(id));
  }

  std::vector<std::string> level_strs;
  level_strs.reserve(levels.size());
  for (uint16_t level : levels) {
    level_strs.push_back(std::to_string(level));
  }

  // Calculate total size needed for the output string
  size_t total_size = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    total_size += ts_strs[i].size() + 1 + id_strs[i].size() + 1 + level_strs[i].size() + 1 + messages[i].size() + 1;
  }

  std::string out;
  out.reserve(total_size);

  // Build the output string
  for (size_t i = 0; i < ids.size(); ++i) {
    out += ts_strs[i];
    out += '|';
    out += id_strs[i];
    out += '|';
    out += level_strs[i];
    out += '|';
    out += messages[i];
    out += '\n';
  }

  // Compute checksum once
  return checksum_text(out);
}