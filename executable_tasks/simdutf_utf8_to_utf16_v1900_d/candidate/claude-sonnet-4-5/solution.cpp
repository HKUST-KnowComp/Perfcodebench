#include "interface.h"

#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Worst case: every byte is ASCII or every 3 bytes becomes 2 UTF-16 units
  size_t max_utf16_length = input.size() * 2;
  std::vector<char16_t> out(max_utf16_length);
  
  size_t actual_length = simdutf::convert_utf8_to_utf16le(
      input.data(), 
      input.size(), 
      out.data()
  );
  
  return checksum_utf16(out.data(), actual_length);
}
