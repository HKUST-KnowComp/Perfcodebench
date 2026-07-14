#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void uint32_to_string(uint32_t value, char* buffer, int& len) {
  if (value == 0) {
    buffer[0] = '0';
    len = 1;
    return;
  }
  
  char temp[10];
  int pos = 0;
  while (value > 0) {
    temp[pos++] = '0' + (value % 10);
    value /= 10;
  }
  
  len = pos;
  for (int i = 0; i < pos; ++i) {
    buffer[i] = temp[pos - 1 - i];
  }
}

inline void uint16_to_string(uint16_t value, char* buffer, int& len) {
  if (value == 0) {
    buffer[0] = '0';
    len = 1;
    return;
  }
  
  char temp[5];
  int pos = 0;
  while (value > 0) {
    temp[pos++] = '0' + (value % 10);
    value /= 10;
  }
  
  len = pos;
  for (int i = 0; i < pos; ++i) {
    buffer[i] = temp[pos - 1 - i];
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  const std::size_t n = ids.size();
  
  // Estimate capacity
  std::size_t estimated_size = 0;
  for (std::size_t i = 0; i < n; ++i) {
    estimated_size += 30 + messages[i].size();
  }
  
  std::string out;
  out.reserve(estimated_size);
  
  uint64_t hash = 0;
  char buffer[10];
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    hash = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < n; ++i) {
      int len;
      
      // timestamp
      uint32_to_string(timestamps[i], buffer, len);
      for (int j = 0; j < len; ++j) {
        out.push_back(buffer[j]);
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[j]));
        hash *= 1099511628211ULL;
      }
      
      out.push_back('|');
      hash ^= static_cast<uint64_t>('|');
      hash *= 1099511628211ULL;
      
      // id
      uint32_to_string(ids[i], buffer, len);
      for (int j = 0; j < len; ++j) {
        out.push_back(buffer[j]);
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[j]));
        hash *= 1099511628211ULL;
      }
      
      out.push_back('|');
      hash ^= static_cast<uint64_t>('|');
      hash *= 1099511628211ULL;
      
      // level
      uint16_to_string(levels[i], buffer, len);
      for (int j = 0; j < len; ++j) {
        out.push_back(buffer[j]);
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[j]));
        hash *= 1099511628211ULL;
      }
      
      out.push_back('|');
      hash ^= static_cast<uint64_t>('|');
      hash *= 1099511628211ULL;
      
      // message
      for (char c : messages[i]) {
        out.push_back(c);
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
      }
      
      out.push_back('\n');
      hash ^= static_cast<uint64_t>('\n');
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}