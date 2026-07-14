#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_utf32_ptr(const char32_t* data, size_t count) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t utf32_len = simdutf::utf32_length_from_utf8(input.data(), input.size());
  std::vector<char32_t> out(utf32_len);
  simdutf::convert_valid_utf8_to_utf32(input.data(), input.size(), out.data());
  return checksum_utf32_ptr(out.data(), utf32_len);
}
