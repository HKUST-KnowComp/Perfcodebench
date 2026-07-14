#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  auto result = simdutf::utf8_to_utf16le(input);
  const std::u16string& u16 = result.string;
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : u16) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}