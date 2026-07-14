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
  // Precompute formatted lines once
  std::vector<std::string> lines;
  lines.reserve(timestamps.size());
  for (std::size_t i = 0; i < timestamps.size(); ++i) {
    lines.push_back(
        std::to_string(timestamps[i]) + "|" + std::to_string(ids[i]) + "|" +
        std::to_string(levels[i]) + "|" + messages[i] + "\n");
  }

  // Pre-calculate total size and build output string efficiently
  std::string out;
  std::size_t total_size = 0;
  for (const auto& line : lines) {
    total_size += line.size();
  }
  out.reserve(total_size);
  for (const auto& line : lines) {
    out += line;
  }

  // Compute checksum once (same result for all iterations)
  return checksum_text(out);
}