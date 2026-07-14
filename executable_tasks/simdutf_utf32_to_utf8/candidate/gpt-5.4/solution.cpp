#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  const size_t count = input.size();
  if (count == 0) {
    return 1469598103934665603ULL;
  }

  const size_t utf8_len = simdutf::utf8_length_from_utf32(reinterpret_cast<const char32_t*>(input.data()), count);

  std::string out;
  out.resize(utf8_len);

  const size_t written = simdutf::convert_valid_utf32_to_utf8(
      reinterpret_cast<const char32_t*>(input.data()), count, out.data());

  return checksum_bytes(out.data(), written);
}
