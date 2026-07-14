#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes_fast(const uint8_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + size;
  const uint8_t* end_unroll = data + (size & ~7);
  
  // Unroll by 8
  while (data < end_unroll) {
    hash ^= static_cast<uint64_t>(data[0]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[3]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[4]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[5]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[6]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[7]);
    hash *= 1099511628211ULL;
    data += 8;
  }
  
  // Handle remainder
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length + frame data
  }
  
  std::vector<uint8_t> out;
  out.reserve(total_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      out.push_back(static_cast<uint8_t>(len));
      out.push_back(static_cast<uint8_t>(len >> 8));
      
      if (!frame.empty()) {
        size_t old_size = out.size();
        out.resize(old_size + frame.size());
        std::memcpy(out.data() + old_size, frame.data(), frame.size());
      }
    }
    
    hash = checksum_bytes_fast(out.data(), out.size());
  }
  
  return hash;
}
