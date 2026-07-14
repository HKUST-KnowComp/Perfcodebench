#include "interface.h"
#include <cstring>

namespace {

inline uint64_t checksum_fused(const uint8_t* data, size_t len, uint64_t hash) {
  const uint8_t* end = data + len;
  const uint8_t* end_unroll = data + (len & ~7);
  
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
  
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();
  }
  
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }
    
    hash = checksum_fused(out.data(), total_size, 1469598103934665603ULL);
  }
  
  return hash;
}
