#include "interface.h"
#include <simdutf.h>

uint64_t transcode_checksum(const std::string& input) {
  // Determine output UTF-16 length
  size_t out_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  
  // Allocate uninitialized buffer (scalar default-init: no zero-fill)
  char16_t* out = new char16_t[out_len];
  
  // Perform SIMD transcoding
  size_t actual = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out);
  
  // Compute checksum over the produced UTF-16 code units
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* p = out;
  const char16_t* end = p + actual;
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  
  delete[] out;
  return hash;
}