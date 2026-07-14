#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_utf16_buffer(const char16_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::u16string out;
  out.resize(utf16_len);
  const size_t written = simdutf::convert_valid_utf8_to_utf16le(
      input.data(), input.size(), reinterpret_cast<char16_t*>(out.data()));
  return checksum_utf16_buffer(reinterpret_cast<const char16_t*>(out.data()), written);
}
