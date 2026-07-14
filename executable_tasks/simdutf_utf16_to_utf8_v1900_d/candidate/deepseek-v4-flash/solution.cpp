#include "interface.h"

#include <cstdint>
#include <string>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // Compute exact UTF-8 byte length for valid UTF-16LE input.
  const size_t utf8_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), input.size());

  std::string result(utf8_len, '\0');

  // Perform the conversion; input is guaranteed valid.
  const size_t written = simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(),
      result.data());

  // Compute FNV-1a 64-bit checksum over the exactly written bytes.
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(result[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}
