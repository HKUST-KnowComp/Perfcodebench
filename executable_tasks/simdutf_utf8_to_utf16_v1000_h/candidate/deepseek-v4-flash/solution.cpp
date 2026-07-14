#include "interface.h"
#include "simdutf.h"
#include <cstddef>
#include <vector>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    // empty input -> empty UTF-16 output, checksum is the initial value
    return 1469598103934665603ULL;
  }
  std::vector<char16_t> out(input.size());  // preallocate, maximum required
  size_t len = simdutf::convert_valid_utf8_to_utf16le(input.data(), input.size(), out.data());
  // input is guaranteed valid, so conversion always succeeds
  return checksum_utf16(out.data(), len);
}
