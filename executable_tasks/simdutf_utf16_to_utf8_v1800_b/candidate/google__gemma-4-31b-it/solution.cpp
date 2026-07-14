#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // Use simdutf to determine the required size for the UTF-8 output
  // The input is u16string, which is UTF-16LE on most platforms where this is tested.
  // We use the provided simdutf library for high-performance transcoding.
  
  // First, calculate the required size
  size_t utf8_len = simdutf::convert_utf16le_to_utf8_size(input.data(), input.size());
  
  // Allocate a buffer for the result
  std::string out;
  out.resize(utf8_len);

  // Perform the actual conversion
  simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data(), utf8_len);

  return checksum_bytes(out.data(), out.size());
}