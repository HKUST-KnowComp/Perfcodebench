#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

// FNV-1a checksum implementation
inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Latin-1 to UTF-8 expansion: max 2 bytes per character
  size_t max_len = input.size() * 2;
  std::vector<char> out(max_len);

  simdutf::latin1_to_utf8_result result = simdutf::latin1_to_utf8(
      reinterpret_cast<const uint8_t*>(input.data()), 
      input.size(), 
      reinterpret_cast<uint8_t*>(out.data())
  );

  return checksum_bytes(out.data(), result.count);
}