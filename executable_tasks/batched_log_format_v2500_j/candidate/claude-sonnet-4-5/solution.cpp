#include "interface.h"

#include <string>
#include <vector>

namespace {

inline char* u32toa(uint32_t value, char* buffer) {
  char* p = buffer;
  if (value == 0) {
    *p++ = '0';
    return p;
  }
  
  char temp[10];
  int len = 0;
  while (value > 0) {
    temp[len++] = '0' + (value % 10);
    value /= 10;
  }
  
  for (int i = len - 1; i >= 0; --i) {
    *p++ = temp[i];
  }
  return p;
}

inline char* u16toa(uint16_t value, char* buffer) {
  char* p = buffer;
  if (value == 0) {
    *p++ = '0';
    return p;
  }
  
  char temp[5];
  int len = 0;
  while (value > 0) {
    temp[len++] = '0' + (value % 10);
    value /= 10;
  }
  
  for (int i = len - 1; i >= 0; --i) {
    *p++ = temp[i];
  }
  return p;
}

uint64_t checksum_text(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  for (; i + 7 < len; i += 8) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+1]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+2]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+3]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+4]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+5]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+6]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+7]));
    hash *= prime;
  }
  
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= prime;
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
  
  char buffer[32];
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    for (size_t i = 0; i < n; ++i) {
      char* p = u32toa(timestamps[i], buffer);
      out.append(buffer, p - buffer);
      out.push_back('|');
      
      p = u32toa(ids[i], buffer);
      out.append(buffer, p - buffer);
      out.push_back('|');
      
      p = u16toa(levels[i], buffer);
      out.append(buffer, p - buffer);
      out.push_back('|');
      
      out.append(messages[i]);
      out.push_back('\n');
    }
    
    hash = checksum_text(out.data(), out.size());
  }
  
  return hash;
}