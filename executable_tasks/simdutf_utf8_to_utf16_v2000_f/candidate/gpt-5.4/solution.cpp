#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  // For valid UTF-8, the number of UTF-16 code units is at most input.size().
  std::u16string utf16;
  utf16.resize(in_len);

  const size_t written = simdutf::convert_valid_utf8_to_utf16le(
      input.data(), in_len, reinterpret_cast<char16_t*>(utf16.data()));
  utf16.resize(written);

  uint64_t hash = 1469598103934665603ULL;
  const char16_t* ptr = reinterpret_cast<const char16_t*>(utf16.data());
  const char16_t* end = ptr + written;
  for (; ptr != end; ++ptr) {
    hash ^= static_cast<uint64_t>(*ptr);
    hash *= 1099511628211ULL;
  }
  return hash;
}
