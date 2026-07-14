#include "interface.h"

#include <vector>
#include <cstring> // for std::memcpy

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += frame.size() + 2; // 2 bytes for length prefix
  }
  
  std::vector<uint8_t> out(total_size * iters);
  uint8_t* out_ptr = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      std::memcpy(out_ptr, &len, sizeof(len));
      out_ptr += sizeof(len);
      std::memcpy(out_ptr, frame.data(), frame.size());
      out_ptr += frame.size();
    }
  }
  
  return checksum_bytes(out.data(), out.size());
}
