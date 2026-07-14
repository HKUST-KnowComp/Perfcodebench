#include "interface.h"

#include <simdutf.h>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }

  const char16_t* input_ptr = input.data();
  size_t input_len = input.size();

  // Calculate required output buffer size
  size_t utf8_len = simdutf::utf8_length_from_utf16le(input_ptr, input_len);

  // Allocate output buffer
  std::vector<char> utf8_output(utf8_len);

  // Perform conversion
  size_t written = simdutf::convert_utf16le_to_utf8(input_ptr, input_len, utf8_output.data());

  // Compute checksum
  return checksum_bytes(utf8_output.data(), written);
}
