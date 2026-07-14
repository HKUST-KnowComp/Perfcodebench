#include "interface.h"
#include <string>
#include <vector>

namespace {

inline char* fast_utoa(char* out, uint32_t val) {
    if (val == 0) { *out++ = '0'; return out; }
    char tmp[12];
    char* p = tmp;
    while (val) { *p++ = '0' + (val % 10); val /= 10; }
    while (p != tmp) { *out++ = *--p; }
    return out;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  std::string buf;
  if (!messages.empty()) {
      buf.reserve(messages.size() * 64);
  }
  
  const size_t n = ids.size();
  uint64_t hash = 0;
  char num_buf[32];
  
  for (int iter = 0; iter < iters; ++iter) {
    buf.clear();
    for (size_t i = 0; i < n; ++i) {
      char* end = fast_utoa(num_buf, timestamps[i]);
      buf.append(num_buf, end - num_buf);
      buf.push_back('|');
      
      end = fast_utoa(num_buf, ids[i]);
      buf.append(num_buf, end - num_buf);
      buf.push_back('|');
      
      end = fast_utoa(num_buf, static_cast<uint32_t>(levels[i]));
      buf.append(num_buf, end - num_buf);
      buf.push_back('|');
      
      buf.append(messages[i]);
      buf.push_back('\n');
    }
    
    hash = 1469598103934665603ULL;
    for (unsigned char c : buf) {
      hash ^= c;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
