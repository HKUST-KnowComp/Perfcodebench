#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>
#include "simdutf.h"

uint64_t transcode_checksum(const std::string& input) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  if (input.empty()) {
    return FNV_OFFSET;
  }

  // Upper bound: number of UTF-16 code units is <= number of UTF-8 bytes
  std::vector<char16_t> out(input.size());
  size_t produced = simdutf::convert_valid_utf8_to_utf16le(input.data(), input.size(), out.data());

  uint64_t hash = FNV_OFFSET;
  const char16_t* p = out.data();
  const char16_t* e = p + produced;
  while (p != e) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= FNV_PRIME;
  }
  return hash;
}
