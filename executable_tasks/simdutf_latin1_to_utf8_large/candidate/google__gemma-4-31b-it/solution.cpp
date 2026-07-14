#include "interface.h"
#include "simdutf.h"
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

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) return 1469598103934665603ULL;

  // Latin-1 to UTF-8 can expand up to 2x
  std::vector<uint8_t> buffer(input.size() * 2);
  
  // simdutf::latin1_to_utf8 returns the number of bytes written to the destination
  size_t written = simdutf::latin1_to_utf8(
      reinterpret_cast<const uint8_t*>(input.data()), 
      input.size(), 
      buffer.data());

  return checksum_bytes(buffer.data(), written);
}