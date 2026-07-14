#include "interface.h"
#include "simdutf.h"

#include <cstddef>

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();

  // Allocate worst-case number of UTF-16 code units (ASCII case => len code units)
  char16_t* out = (len != 0) ? new char16_t[len] : nullptr;

  // Input is guaranteed valid UTF-8; use the fast path
  size_t written = simdutf::convert_valid_utf8_to_utf16(src, len, out);

  // FNV-1a 64-bit hash over the exact UTF-16 code unit sequence
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }

  delete[] out;
  return hash;
}
