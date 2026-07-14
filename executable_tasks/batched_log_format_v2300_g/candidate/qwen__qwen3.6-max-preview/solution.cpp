#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace {

inline void hash_update(uint64_t& h, unsigned char c) {
    h ^= c;
    h *= 1099511628211ULL;
}

inline int format_uint(char* buf, uint32_t v) {
    if (v == 0) { buf[0] = '0'; return 1; }
    char tmp[12];
    int len = 0;
    while (v) { tmp[len++] = '0' + (v % 10); v /= 10; }
    for (int i = 0; i < len; ++i) buf[i] = tmp[len - 1 - i];
    return len;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  const size_t n = timestamps.size();
  uint64_t hash = 0;
  char num_buf[16];
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      int len1 = format_uint(num_buf, timestamps[i]);
      for (int j = 0; j < len1; ++j) hash_update(hash, static_cast<unsigned char>(num_buf[j]));
      hash_update(hash, '|');
      
      int len2 = format_uint(num_buf, ids[i]);
      for (int j = 0; j < len2; ++j) hash_update(hash, static_cast<unsigned char>(num_buf[j]));
      hash_update(hash, '|');
      
      int len3 = format_uint(num_buf, levels[i]);
      for (int j = 0; j < len3; ++j) hash_update(hash, static_cast<unsigned char>(num_buf[j]));
      hash_update(hash, '|');
      
      const std::string& msg = messages[i];
      const unsigned char* mdata = reinterpret_cast<const unsigned char*>(msg.data());
      const unsigned char* mend = mdata + msg.size();
      while (mdata < mend) {
        hash_update(hash, *mdata++);
      }
      hash_update(hash, '\n');
    }
  }
  return hash;
}