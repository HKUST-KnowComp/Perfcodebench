#include "interface.h"

#include <cstring>
#include <vector>

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  // Unroll by 4 for better ILP
  for (; i + 3 < len; i += 4) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 3]);
    hash *= prime;
  }
  
  // Handle remainder
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
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
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // Inline length writing
      *ptr++ = static_cast<uint8_t>(len);
      *ptr++ = static_cast<uint8_t>(len >> 8);
      // Bulk copy frame data
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }
    
    hash = checksum_bytes(out.data(), total_size);
  }
  
  return hash;
}
