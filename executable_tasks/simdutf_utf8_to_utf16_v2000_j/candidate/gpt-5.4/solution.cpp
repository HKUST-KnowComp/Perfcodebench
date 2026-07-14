#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  const char* data = input.data();
  const size_t len = input.size();

  // Fast path for valid UTF-8 using simdutf's optimized transcoder.
  // For valid UTF-8, the UTF-16 length is at most the UTF-8 byte length.
  std::u16string utf16;
  utf16.resize(len);
  const size_t written = simdutf::convert_valid_utf8_to_utf16le(data, len, utf16.data());
  utf16.resize(written);

  // FNV-1a over UTF-16 code units.
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* p = utf16.data();
  const char16_t* end = p + utf16.size();
  for (; p != end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL;
  }
  return hash;
}
