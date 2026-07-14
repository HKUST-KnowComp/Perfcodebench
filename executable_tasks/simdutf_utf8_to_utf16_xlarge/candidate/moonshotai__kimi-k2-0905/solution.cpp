#include "interface.h"

#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  // simdutf gives count in UTF-16 code units, so we allocate directly to size.
  std::vector<char16_t> out(simdutf::utf16_length_from_utf8(input.data(), input.size()));

  [[maybe_unused]] size_t written = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), out.data());

  // Checksum like before, but unrolled for speed
  const char16_t* p = out.data();
  const char16_t* const end = p + out.size();
  uint64_t hash = 1469598103934665603ULL;
  for (; p < end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL;
  }
  return hash;
}