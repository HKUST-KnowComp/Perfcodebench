#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_utf16_ptr(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  const size_t max_units = simdutf::utf16_length_from_utf8(input.data(), in_len);
  char16_t* buffer = new char16_t[max_units];

  const size_t written = simdutf::convert_valid_utf8_to_utf16le(input.data(), in_len, buffer);
  const uint64_t hash = checksum_utf16_ptr(buffer, written);

  delete[] buffer;
  return hash;
}
