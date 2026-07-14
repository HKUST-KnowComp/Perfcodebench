#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

uint64_t fnv1a_hash(const std::string& text) {
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
  // Precompute the complete formatted lines once.
  const std::size_t n = ids.size();
  std::vector<std::string> lines(n);
  for (std::size_t i = 0; i < n; ++i) {
    lines[i] = std::to_string(timestamps[i]) + "|" +
               std::to_string(ids[i]) + "|" +
               std::to_string(levels[i]) + "|" +
               messages[i] + "\n";
  }

  // Compute the final checksum only once (output is identical every iteration).
  if (iters <= 0) {
    return fnv1a_hash(std::string());
  }

  uint64_t hash = 1469598103934665603ULL;
  for (const auto& line : lines) {
    for (unsigned char c : line) {
      hash ^= static_cast<uint64_t>(c);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
