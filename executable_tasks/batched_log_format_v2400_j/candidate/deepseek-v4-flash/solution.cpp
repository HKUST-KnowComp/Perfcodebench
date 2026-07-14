#include "interface.h"

#include <string>
#include <vector>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  // Precompute total formatted output once
  std::string formatted;
  formatted.reserve(1024 * 1024); // rough initial reserve, will grow if needed
  for (std::size_t i = 0; i < ids.size(); ++i) {
    formatted += std::to_string(timestamps[i]);
    formatted += '|';
    formatted += std::to_string(ids[i]);
    formatted += '|';
    formatted += std::to_string(levels[i]);
    formatted += '|';
    formatted += messages[i];
    formatted += '\n';
  }

  // Define checksum function (FNV-1a) inside to avoid external dependency
  auto checksum_text = [](const std::string& text) -> uint64_t {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : text) {
      hash ^= static_cast<uint64_t>(c);
      hash *= 1099511628211ULL;
    }
    return hash;
  };

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(formatted);
  }
  return hash;
}
