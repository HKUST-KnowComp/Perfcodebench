#include "interface.h"
#include <string>
#include <simdutf.h>

namespace {

uint64_t fnv1a_checksum(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // Determine exact UTF-8 output length (input is assumed valid)
  auto result = simdutf::calculate_utf8_length_from_utf16le(input.data(), input.size());
  size_t utf8_len = result.count;
  // Allocate buffer (no extra null terminator needed for checksum)
  std::string out(utf8_len, '\0');
  // Transcode UTF-16LE to UTF-8 into the buffer
  size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data());
  // Compute checksum over the written bytes
  return fnv1a_checksum(out.data(), written);
}
