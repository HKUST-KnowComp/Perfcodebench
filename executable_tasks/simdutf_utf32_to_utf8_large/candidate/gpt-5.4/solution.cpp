#include "interface.h"

#include <simdutf.h>

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

static inline size_t utf8_length_from_utf32(const std::vector<char32_t>& input) {
  size_t total = 0;
  for (char32_t c : input) {
    uint32_t cp = static_cast<uint32_t>(c);
    total += 1 + static_cast<size_t>(cp > 0x7F) + static_cast<size_t>(cp > 0x7FF) + static_cast<size_t>(cp > 0xFFFF);
  }
  return total;
}

}  // namespace

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  const size_t utf8_len = utf8_length_from_utf32(input);
  std::string out(utf8_len, '\0');

  size_t written = simdutf::convert_valid_utf32_to_utf8(
      reinterpret_cast<const char32_t*>(input.data()), input.size(), out.data());

  if (written != utf8_len) {
    out.resize(written);
    return checksum_bytes(out.data(), out.size());
  }

  return checksum_bytes(out.data(), utf8_len);
}
