#include "interface.h"

#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precalculate total buffer size to avoid any dynamic reallocations
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }

  // Prepack the frame buffer once, since it never changes across iterations
  std::vector<uint8_t> out;
  out.reserve(total_size);
  for (const auto& frame : frames) {
    const uint16_t frame_len = static_cast<uint16_t>(frame.size());
    out.push_back(static_cast<uint8_t>(frame_len));
    out.push_back(static_cast<uint8_t>(frame_len >> 8));
    out.insert(out.end(), frame.begin(), frame.end());
  }

  // Run checksum repeatedly on the cached, immutable buffer
  uint64_t hash = 0;
  const uint8_t* data = out.data();
  const size_t len = out.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(data, len);
  }
  return hash;
}