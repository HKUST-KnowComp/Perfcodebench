#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes_inline(const uint8_t* data, size_t len, uint64_t hash) {
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length + frame data
  }
  
  std::vector<uint8_t> out;
  out.reserve(total_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    hash = 1469598103934665603ULL;
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      // Write length and compute checksum inline
      uint8_t len_bytes[2];
      len_bytes[0] = static_cast<uint8_t>(len);
      len_bytes[1] = static_cast<uint8_t>(len >> 8);
      
      out.push_back(len_bytes[0]);
      out.push_back(len_bytes[1]);
      
      hash ^= static_cast<uint64_t>(len_bytes[0]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(len_bytes[1]);
      hash *= 1099511628211ULL;
      
      // Write frame data and compute checksum inline
      if (!frame.empty()) {
        size_t old_size = out.size();
        out.resize(old_size + frame.size());
        std::memcpy(out.data() + old_size, frame.data(), frame.size());
        
        hash = checksum_bytes_inline(reinterpret_cast<const uint8_t*>(frame.data()), frame.size(), hash);
      }
    }
  }
  
  return hash;
}