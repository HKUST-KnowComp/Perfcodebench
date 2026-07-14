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

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  // Precompute total size needed for one iteration's output
  size_t total_size = 0;
  for (std::size_t i = 0; i < ids.size(); ++i) {
    total_size += std::to_string(timestamps[i]).length();
    total_size += 1; // |
    total_size += std::to_string(ids[i]).length();
    total_size += 1; // |
    total_size += std::to_string(levels[i]).length();
    total_size += 1; // |
    total_size += messages[i].length();
    total_size += 1; // \n
  }

  std::string out;
  out.reserve(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (std::size_t i = 0; i < ids.size(); ++i) {
      out += std::to_string(timestamps[i]);
      out += '|';
      out += std::to_string(ids[i]);
      out += '|';
      out += std::to_string(levels[i]);
      out += '|';
      out += messages[i];
      out += '\n';
    }
    hash = checksum_text(out);
  }
  return hash;
}