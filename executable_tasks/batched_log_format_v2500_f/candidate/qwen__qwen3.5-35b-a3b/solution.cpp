#include "interface.h"

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

// Fast integer to string conversion without allocations
inline void fast_to_string(uint32_t value, std::string& out) {
  if (value == 0) {
    out += '0';
    return;
  }
  
  char buffer[12];
  int pos = 0;
  while (value > 0) {
    buffer[pos++] = '0' + (value % 10);
    value /= 10;
  }
  // Reverse and append
  for (int i = pos - 1; i >= 0; --i) {
    out += buffer[i];
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  // Pre-allocate output buffer to avoid reallocations
  // Estimate: ~15 chars per field + 4 separators + newline per record
  std::string out;
  out.reserve(timestamps.size() * (15 + 15 + 5 + 1 + 1) * iters);
  
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < ids.size(); ++i) {
      fast_to_string(timestamps[i], out);
      out += '|';
      fast_to_string(ids[i], out);
      out += '|';
      fast_to_string(levels[i], out);
      out += '|';
      out += messages[i];
      out += '\n';
    }
  }
  
  return checksum_text(out);
}