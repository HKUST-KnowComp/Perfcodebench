#include "interface.h"

#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      
      hash ^= static_cast<uint64_t>(len & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(len >> 8);
      hash *= FNV_PRIME;
      
      for (unsigned char c : frame) {
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_PRIME;
      }
    }
  }
  return hash;
}
