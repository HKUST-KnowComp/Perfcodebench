#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  size_t end8 = len & ~size_t(7);
  
  for (; i < end8; i += 8) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 7]);
    hash *= prime;
  }
  
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precompute total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }
  
  // Preallocate buffer once
  std::vector<uint8_t> out(total_size);
  uint8_t* buf = out.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = buf;
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // Write length prefix (little-endian)
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      
      // Copy frame data
      std::memcpy(ptr, frame.data(), frame.size());
      ptr += frame.size();
    }
    
    hash = checksum_bytes(buf, total_size);
  }
  
  return hash;
}