#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void append_uint32(std::string& s, uint32_t val) {
  if (val == 0) {
    s.push_back('0');
    return;
  }
  char buf[10];
  int pos = 10;
  while (val > 0) {
    buf[--pos] = '0' + (val % 10);
    val /= 10;
  }
  s.append(buf + pos, 10 - pos);
}

inline void append_uint16(std::string& s, uint16_t val) {
  if (val == 0) {
    s.push_back('0');
    return;
  }
  char buf[5];
  int pos = 5;
  while (val > 0) {
    buf[--pos] = '0' + (val % 10);
    val /= 10;
  }
  s.append(buf + pos, 5 - pos);
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
  
  // Estimate output size
  std::size_t est_size = 0;
  for (std::size_t i = 0; i < ids.size(); ++i) {
    est_size += 30 + messages[i].size();
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    out.reserve(est_size);
    
    hash = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < ids.size(); ++i) {
      std::size_t start_pos = out.size();
      
      append_uint32(out, timestamps[i]);
      out.push_back('|');
      append_uint32(out, ids[i]);
      out.push_back('|');
      append_uint16(out, levels[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
      
      // Compute checksum inline
      for (std::size_t j = start_pos; j < out.size(); ++j) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[j]));
        hash *= 1099511628211ULL;
      }
    }
  }
  
  return hash;
}