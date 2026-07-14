#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }
  
  std::vector<uint8_t> out;
  out.reserve(total_size);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      // Inline length writing
      out.push_back(static_cast<uint8_t>(len));
      out.push_back(static_cast<uint8_t>(len >> 8));
      
      // Use memcpy for bulk data transfer
      size_t old_size = out.size();
      out.resize(old_size + frame.size());
      std::memcpy(out.data() + old_size, frame.data(), frame.size());
    }
    
    hash = checksum_bytes(out.data(), out.size());
  }
  return hash;
}