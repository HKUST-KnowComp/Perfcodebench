#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_utf16_ptr(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();

  // For valid UTF-8, UTF-16 output length is at most the number of input bytes.
  std::u16string out;
  out.resize(len);

  const size_t written = simdutf::convert_valid_utf8_to_utf16le(src, len, out.data());
  out.resize(written);

  return checksum_utf16_ptr(out.data(), written);
}
