#include "interface.h"
#include <cstring>
#include <vector>

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
  size_t total_size = 0;
  std::vector<std::pair<uint16_t, const char*>> frame_info;
  frame_info.reserve(frames.size());
  for (const auto& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    frame_info.emplace_back(len, frame.data());
    total_size += 2 + len;
  }

  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    for (const auto& info : frame_info) {
      *dst++ = static_cast<uint8_t>(info.first);
      *dst++ = static_cast<uint8_t>(info.first >> 8);
      std::memcpy(dst, info.second, info.first);
      dst += info.first;
    }
    hash = checksum_bytes(out.data(), total_size);
  }
  return hash;
}