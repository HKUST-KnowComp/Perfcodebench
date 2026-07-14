#include "interface.h"

#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  std::vector<uint8_t> out;
  uint64_t hash = 0;

  // Precompute total size to avoid reallocations
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }
  out.reserve(total_size);

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (const std::string& frame : frames) {
      uint16_t len = frame.size();
      out.push_back(len & 0xFF);
      out.push_back((len >> 8) & 0xFF);
      out.insert(out.end(), frame.begin(), frame.end());
    }
    hash = checksum_bytes(out);
  }
  return hash;
}