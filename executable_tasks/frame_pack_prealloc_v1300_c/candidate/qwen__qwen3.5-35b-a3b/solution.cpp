#include "interface.h"

#include <vector>
#include <string>

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
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }

  std::vector<uint8_t> out;
  out.reserve(total_size);

  for (const auto& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    out.push_back(static_cast<uint8_t>(len));
    out.push_back(static_cast<uint8_t>(len >> 8));
    out.insert(out.end(), frame.begin(), frame.end());
  }

  return checksum_bytes(out);
}