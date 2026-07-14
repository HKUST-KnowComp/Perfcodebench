#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void append_uint32(std::string& s, uint32_t val) {
  char buf[10];
  char* p = buf + 10;
  do {
    *--p = '0' + (val % 10);
    val /= 10;
  } while (val);
  s.append(p, buf + 10 - p);
}

inline void append_uint16(std::string& s, uint16_t val) {
  char buf[5];
  char* p = buf + 5;
  do {
    *--p = '0' + (val % 10);
    val /= 10;
  } while (val);
  s.append(p, buf + 5 - p);
}

inline uint64_t update_hash(uint64_t hash, const char* data, size_t len) {
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
  std::string out;
  uint64_t hash = 0;
  
  size_t n = ids.size();
  size_t estimated_size = 0;
  for (size_t i = 0; i < n; ++i) {
    estimated_size += 30 + messages[i].size();
  }
  out.reserve(estimated_size);
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < n; ++i) {
      size_t start_pos = out.size();
      
      append_uint32(out, timestamps[i]);
      out.push_back('|');
      append_uint32(out, ids[i]);
      out.push_back('|');
      append_uint16(out, levels[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
      
      hash = update_hash(hash, out.data() + start_pos, out.size() - start_pos);
    }
  }
  return hash;
}
