#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* data = reinterpret_cast<const char16_t*>(input.data());
  const size_t len = input.size();

  // For valid UTF-16, UTF-8 output is at most 3 bytes per UTF-16 code unit.
  std::string out;
  out.resize(len * 3);

  const size_t written = simdutf::convert_valid_utf16le_to_utf8(data, len, out.data());
  out.resize(written);

  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
  const unsigned char* end = p + written;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}
