#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_bytes(const uint8_t* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes(nullptr, 0);
  }
  // Compute exact capacity needed for UTF-8 output
  std::size_t required_capacity = simdutf::utf8_length_from_utf16le(input.data(), input.size());
  std::string out(required_capacity, '\0');
  // Perform conversion; writes exactly required_capacity bytes (input is valid)
  std::size_t actual_len = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data());
  out.resize(actual_len);  // Ensure length matches written bytes
  return checksum_bytes(reinterpret_cast<const uint8_t*>(out.data()), actual_len);
}
