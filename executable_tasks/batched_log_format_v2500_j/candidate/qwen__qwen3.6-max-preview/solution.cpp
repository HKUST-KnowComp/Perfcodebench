#include "interface.h"

#include <string>
#include <vector>
#include <cstring>

namespace {

inline char* fast_utoa(char* buf, uint32_t val) {
    if (val == 0) { *buf++ = '0'; return buf; }
    char tmp[12];
    char* p = tmp;
    while (val >= 100) {
        uint32_t c = val % 100;
        val /= 100;
        *p++ = char('0' + c % 10);
        *p++ = char('0' + c / 10);
    }
    if (val >= 10) {
        *p++ = char('0' + val % 10);
        *p++ = char('0' + val / 10);
    } else {
        *p++ = char('0' + val);
    }
    while (p != tmp) *buf++ = *--p;
    return buf;
}

inline uint64_t checksum_text(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
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
  size_t n = ids.size();

  size_t total_len = 0;
  for (size_t i = 0; i < n; ++i) {
      total_len += 29 + messages[i].size(); 
  }

  std::vector<char> buffer(total_len);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
      char* dst = buffer.data();
      for (size_t i = 0; i < n; ++i) {
          dst = fast_utoa(dst, timestamps[i]);
          *dst++ = '|';
          dst = fast_utoa(dst, ids[i]);
          *dst++ = '|';
          dst = fast_utoa(dst, levels[i]);
          *dst++ = '|';
          const std::string& msg = messages[i];
          std::memcpy(dst, msg.data(), msg.size());
          dst += msg.size();
          *dst++ = '\n';
      }
      size_t actual_len = dst - buffer.data();
      hash = checksum_text(buffer.data(), actual_len);
  }
  return hash;
}