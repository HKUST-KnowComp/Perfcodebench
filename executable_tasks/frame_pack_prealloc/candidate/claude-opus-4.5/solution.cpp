#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-compute total size needed: 2 bytes length prefix per frame + frame data
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();
  }
  
  // Pre-allocate buffer once
  std::vector<uint8_t> out(total_size);
  uint8_t* const buf = out.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack frames into buffer
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
    
    // Compute FNV-1a checksum
    hash = 1469598103934665603ULL;
    const uint8_t* end = buf + total_size;
    for (const uint8_t* p = buf; p < end; ++p) {
      hash ^= static_cast<uint64_t>(*p);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}