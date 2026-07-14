#include "interface.h"
#include "simdutf.h"

namespace {

uint64_t checksum_utf32(const char32_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t utf32_length = simdutf::utf8_to_utf32::validate_utf8(input.data(), input.size());
  if (simdutf::utf8_to_utf32::validate_utf8(input.data(), input.size()) == 0) {
    // Handle validation error, for simplicity, assume input is valid UTF-8
    return 0;
  }
  std::vector<char32_t> out(utf32_length);
  simdutf::utf8_to_utf32::convert_utf8_to_utf32(input.data(), input.size(), out.data());
  return checksum_utf32(out.data(), out.size());
}