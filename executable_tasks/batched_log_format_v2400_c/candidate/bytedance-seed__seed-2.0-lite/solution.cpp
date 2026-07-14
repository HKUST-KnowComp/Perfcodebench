#include "interface.h"

#include <string>
#include <vector>
#include <charconv>

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
  std::string out;
  uint64_t hash = 0;

  if (n > 0) {
    // Precalculate exact total size to avoid reallocations
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
      // Count digits for all numeric values
      uint32_t ts = timestamps[i]; int cnt_ts = 0;
      do { cnt_ts++; ts /= 10; } while (ts);
      uint32_t id = ids[i]; int cnt_id = 0;
      do { cnt_id++; id /= 10; } while (id);
      uint16_t lvl = levels[i]; int cnt_lvl = 0;
      do { cnt_lvl++; lvl /= 10; } while (lvl);
      total_size += cnt_ts + cnt_id + cnt_lvl + messages[i].size() + 4; // 3x | + 1x \n
    }

    out.reserve(total_size);
    char buf[16]; // Sufficient for max length of all input numeric types
    for (size_t i = 0; i < n; ++i) {
      // Write timestamp
      auto [ptr, ec] = std::to_chars(buf, buf + 16, timestamps[i]);
      out.append(buf, ptr - buf);
      out += '|';
      // Write ID
      std::tie(ptr, ec) = std::to_chars(buf, buf + 16, ids[i]);
      out.append(buf, ptr - buf);
      out += '|';
      // Write log level
      std::tie(ptr, ec) = std::to_chars(buf, buf + 16, levels[i]);
      out.append(buf, ptr - buf);
      out += '|';
      // Write message and newline
      out += messages[i];
      out += '\n';
    }
  }

  // Hot loop: only run checksum, no formatting work per iteration
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_text(out);
  }
  return hash;
}