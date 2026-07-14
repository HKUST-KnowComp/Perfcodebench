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
  const char* src = input.data();
  const size_t src_len = input.size();

  const size_t utf16_len = simdutf::utf16_length_from_utf8(src, src_len);
  char16_t* out = new char16_t[utf16_len];

  const size_t written = simdutf::convert_valid_utf8_to_utf16le(src, src_len, out);
  uint64_t result = checksum_utf16_ptr(out, written);

  delete[] out;
  return result;
}
