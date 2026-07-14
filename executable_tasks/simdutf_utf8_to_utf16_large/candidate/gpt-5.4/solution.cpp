#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  const char* data = input.data();
  const size_t len = input.size();

  // Fast path: compute required UTF-16 length, transcode once into a tightly
  // sized buffer, then hash directly from that buffer.
  const size_t utf16_len = simdutf::utf16_length_from_utf8(data, len);
  std::u16string out;
  out.resize(utf16_len);

  const size_t written = simdutf::convert_valid_utf8_to_utf16le(
      data, len, reinterpret_cast<char16_t*>(out.data()));

  uint64_t hash = 1469598103934665603ULL;
  const char16_t* ptr = reinterpret_cast<const char16_t*>(out.data());
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
