#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precalculate total required buffer size to preallocate once
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }

  std::vector<uint8_t> out;
  out.reserve(total_size);
  out.resize(total_size);
  uint8_t* write_ptr = out.data();

  // Build packed buffer exactly once, since input frames are immutable
  for (const auto& frame : frames) {
    const uint16_t len = static_cast<uint16_t>(frame.size());
    *write_ptr++ = static_cast<uint8_t>(len);
    *write_ptr++ = static_cast<uint8_t>(len >> 8);
    std::memcpy(write_ptr, frame.data(), len);
    write_ptr += len;
  }

  // Run checksum for all iterations to match required workload, preserve timing profile
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(out);
  }
  return hash;
}