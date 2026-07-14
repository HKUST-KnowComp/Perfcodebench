#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + len;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total = 0;
  for (const std::string& f : frames) total += 2 + f.size();

  std::vector<uint8_t> out;
  out.reserve(total);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.resize(total);
    size_t pos = 0;
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      out[pos++] = static_cast<uint8_t>(len);
      out[pos++] = static_cast<uint8_t>(len >> 8);
      std::memcpy(&out[pos], frame.data(), frame.size());
      pos += frame.size();
    }
    hash = checksum_bytes(out.data(), out.size());
  }
  return hash;
}