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
  std::vector<std::string> lines;
  std::string out;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    lines.clear();
    for (std::size_t i = 0; i < ids.size(); ++i) {
      lines.push_back(
          std::to_string(timestamps[i]) + "|" + std::to_string(ids[i]) + "|" +
          std::to_string(levels[i]) + "|" + messages[i] + "\n");
    }
    out.clear();
    for (const std::string& line : lines) {
      out += line;
    }
    hash = checksum_text(out);
  }
  return hash;
}
