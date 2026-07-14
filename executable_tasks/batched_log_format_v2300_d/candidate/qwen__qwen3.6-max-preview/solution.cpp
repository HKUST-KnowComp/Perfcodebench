#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_text(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline void append_u32(std::string& out, uint32_t v) {
    if (v == 0) {
        out.push_back('0');
        return;
    }
    char buf[10];
    int len = 0;
    while (v > 0) {
        buf[len++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        out.push_back(buf[i]);
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  std::string out;
  size_t n = timestamps.size();
  
  // Pre-calculate capacity to avoid reallocations during formatting
  size_t cap = 0;
  for (size_t i = 0; i < n; ++i) {
      cap += messages[i].size() + 32; 
  }
  out.reserve(cap);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (size_t i = 0; i < n; ++i) {
      append_u32(out, timestamps[i]);
      out.push_back('|');
      append_u32(out, ids[i]);
      out.push_back('|');
      append_u32(out, levels[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
    }
    hash = checksum_text(out.data(), out.size());
  }
  return hash;
}
