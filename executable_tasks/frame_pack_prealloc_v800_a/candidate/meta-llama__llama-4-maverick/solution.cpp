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

void write_len(std::vector<uint8_t>& out, size_t pos, uint16_t v) {
  out[pos] = static_cast<uint8_t>(v);
  out[pos + 1] = static_cast<uint8_t>(v >> 8);
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += frame.size() + 2;
  }
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t pos = 0;
    for (const std::string& frame : frames) {
      write_len(out, pos, static_cast<uint16_t>(frame.size()));
      pos += 2;
      std::copy(frame.begin(), frame.end(), out.begin() + pos);
      pos += frame.size();
    }
    hash = checksum_bytes(out);
  }
  return hash;
}
