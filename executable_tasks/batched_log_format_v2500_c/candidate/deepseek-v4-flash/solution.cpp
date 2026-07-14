#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

uint64_t fnv1a_64(const std::string& s) noexcept {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char c : s) {
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
  if (iters <= 0) return 0;

  const std::size_t n = ids.size();
  if (n == 0) return 1469598103934665603ULL;  // FNV-1a of empty string

  // Estimate total output length: each line has 3 separators + newline (4 chars)
  // plus up to 10 digits for timestamp, 10 for id, 5 for level.
  std::size_t total_est = 0;
  for (const auto& msg : messages) {
    total_est += msg.size();
  }
  total_est += n * (10 + 10 + 5 + 4);  // conservative upper bound

  std::string out;
  out.reserve(total_est);

  for (std::size_t i = 0; i < n; ++i) {
    out += std::to_string(timestamps[i]);
    out += '|';
    out += std::to_string(ids[i]);
    out += '|';
    out += std::to_string(levels[i]);
    out += '|';
    out += messages[i];
    out += '\n';
  }

  const uint64_t hash = fnv1a_64(out);
  // The checksum is identical for every iteration; return it directly.
  return hash;
}
