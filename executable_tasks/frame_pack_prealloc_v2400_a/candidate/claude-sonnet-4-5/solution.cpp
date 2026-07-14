#include "interface.h"

#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    uint64_t h = 1469598103934665603ULL;
    
    // Fused packing and checksumming
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      // Write length prefix and update checksum
      uint8_t b0 = static_cast<uint8_t>(len);
      uint8_t b1 = static_cast<uint8_t>(len >> 8);
      ptr[0] = b0;
      ptr[1] = b1;
      
      h ^= static_cast<uint64_t>(b0);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b1);
      h *= 1099511628211ULL;
      
      ptr += 2;
      
      // Copy frame data and update checksum
      const uint8_t* src = reinterpret_cast<const uint8_t*>(frame.data());
      std::memcpy(ptr, src, len);
      
      // Unrolled checksum loop
      size_t i = 0;
      for (; i + 3 < len; i += 4) {
        h ^= static_cast<uint64_t>(ptr[i]);
        h *= 1099511628211ULL;
        h ^= static_cast<uint64_t>(ptr[i + 1]);
        h *= 1099511628211ULL;
        h ^= static_cast<uint64_t>(ptr[i + 2]);
        h *= 1099511628211ULL;
        h ^= static_cast<uint64_t>(ptr[i + 3]);
        h *= 1099511628211ULL;
      }
      
      // Handle remaining bytes
      for (; i < len; ++i) {
        h ^= static_cast<uint64_t>(ptr[i]);
        h *= 1099511628211ULL;
      }
      
      ptr += len;
    }
    
    hash = h;
  }
  
  return hash;
}