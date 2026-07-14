#include "interface.h"

#include <simdutf.h>
#include <string>

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
  // Pre-allocate maximum possible UTF-8 length (3 bytes per UTF-16 code unit)
  std::string out(input.size() * 3, '\0');
  // Convert valid UTF-16LE to UTF-8 using SIMD-accelerated routine
  size_t utf8_len = simdutf::convert_valid_utf16le_to_utf8(
      input.data(), input.size(), out.data());
  out.resize(utf8_len);
  return checksum_bytes(out);
}