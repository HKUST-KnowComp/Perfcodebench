#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t compute_fnv1a(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* data = input.data();
  size_t len = input.size();

  // Max UTF-8 length for UTF-16 is 3x the number of code units
  size_t max_len = simdutf::utf8_length_from_utf16(data, len);
  std::vector<char> buffer(max_len);

  size_t actual_len = simdutf::convert_utf16_to_utf8(data, len, buffer.data());

  return compute_fnv1a(buffer.data(), actual_len);
}