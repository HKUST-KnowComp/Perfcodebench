#include "interface.h"
#include <simdutf.h>

extern "C" {

uint64_t transcode_checksum(const std::string& input) {
  // simdutf guarantees the output will be valid UTF-16LE and reports the exact
  // number of 16-bit code units produced.
  size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());

  // Stack buffer for the common case; falls back to heap for very large inputs.
  constexpr size_t STACK_LIMIT = 4096;
  char16_t stack_buf[STACK_LIMIT];
  char16_t* out = (utf16_len <= STACK_LIMIT) ? stack_buf
                                               : new char16_t[utf16_len];

  // Fast SIMD conversion.
  size_t written = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), reinterpret_cast<char16_t*>(out));

  // Compute checksum (FNV-1a 64-bit).
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* p = out;
  const char16_t* end = p + written;
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }

  if (out != stack_buf) delete[] out;
  return hash;
}

} // extern "C"