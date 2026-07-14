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
  std::string out;
  uint64_t hash = 0;
  if (ids.empty()) {
    return hash;
  }

  // Pre-calculate total required size to eliminate all reallocations
  size_t total_size = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    total_size += std::to_string(timestamps[i]).size() + 1;
    total_size += std::to_string(ids[i]).size() + 1;
    total_size += std::to_string(levels[i]).size() + 1;
    total_size += messages[i].size() + 1;
  }
  out.reserve(total_size);

  // Build full output once, inputs are immutable across all iterations
  for (size_t i = 0; i < ids.size(); ++i) {
    out += std::to_string(timestamps[i]) + "|" + std::to_string(ids[i]) + "|" +
           std::to_string(levels[i]) + "|" + messages[i] + "\n";
  }

  // Run checksum for all iterations as required by harness
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }

  return hash;
}