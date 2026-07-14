#include "interface.h"

#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-calculate total size
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();
  }
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    hash = 1469598103934665603ULL;
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      // Write length prefix and update hash
      uint8_t b0 = static_cast<uint8_t>(len);
      uint8_t b1 = static_cast<uint8_t>(len >> 8);
      *ptr++ = b0;
      *ptr++ = b1;
      
      hash ^= static_cast<uint64_t>(b0);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(b1);
      hash *= 1099511628211ULL;
      
      // Copy frame data and update hash
      const uint8_t* src = reinterpret_cast<const uint8_t*>(frame.data());
      size_t frame_len = frame.size();
      std::memcpy(ptr, src, frame_len);
      
      // Unrolled checksum for frame data
      size_t i = 0;
      for (; i + 3 < frame_len; i += 4) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(ptr[i + 1]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(ptr[i + 2]);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(ptr[i + 3]);
        hash *= 1099511628211ULL;
      }
      for (; i < frame_len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
      }
      
      ptr += frame_len;
    }
  }
  
  return hash;
}