#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  const char* data = input.data();
  const size_t len = input.size();

  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  // Fast path: transcode directly to a temporary UTF-32 buffer using simdutf,
  // then compute the checksum in one linear pass.
  // For valid UTF-8, utf32_length_from_utf8 gives the exact number of code points.
  const size_t utf32_len = simdutf::utf32_length_from_utf8(data, len);
  if (utf32_len == 0) {
    return hash;
  }

  char32_t* buffer = new char32_t[utf32_len];
  const size_t written = simdutf::convert_valid_utf8_to_utf32(data, len, buffer);

  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(buffer[i]);
    hash *= kPrime;
  }

  delete[] buffer;
  return hash;
}
