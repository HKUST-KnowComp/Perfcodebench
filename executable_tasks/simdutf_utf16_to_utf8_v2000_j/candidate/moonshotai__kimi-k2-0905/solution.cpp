#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstddef>

uint64_t transcode_checksum(const std::u16string& input) {
  // Ask simdutf for the exact number of bytes required.
  size_t expected_utf8_length =
      simdutf::utf8_length_from_utf16(reinterpret_cast<const char16_t*>(input.data()),
                                      input.size());
  std::vector<char> utf8(expected_utf8_length);

  // Perform the actual transcoding.
  size_t written = simdutf::convert_utf16_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()),
      input.size(),
      utf8.data());

  // Compute the checksum over the UTF-8 byte sequence.
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(utf8[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}