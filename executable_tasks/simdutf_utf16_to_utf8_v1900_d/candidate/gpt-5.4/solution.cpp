#include "interface.h"

#include <cstdint>
#include <string>

#include "simdutf.h"

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  const size_t utf8_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), in_len);

  std::string out;
  out.resize(utf8_len);

  const size_t written = simdutf::convert_valid_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), in_len, out.data());

  return checksum_bytes(out.data(), written);
}
