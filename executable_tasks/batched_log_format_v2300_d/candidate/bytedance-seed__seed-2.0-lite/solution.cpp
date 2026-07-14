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
  // Precalculate total required buffer size to avoid reallocations
  size_t total_size = 0;
  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    // Max possible length for all numeric fields + separators + message + newline
    total_size += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
  }

  // Build full log buffer exactly once (input is immutable across all iterations)
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

  // Run checksum for all required iterations
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }
  return hash;
}