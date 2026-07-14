#include "interface.h"
#include <string>
#include <simdutf.h>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // Calculate exact required UTF-8 output size
  const size_t utf8_byte_count = simdutf::utf8_length_from_utf16le(input.data(), input.size());
  std::string out;
  out.resize(utf8_byte_count);
  // Use SIMD-accelerated valid UTF-16LE to UTF-8 transcoding
  simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data());
  return checksum_bytes(out);
}