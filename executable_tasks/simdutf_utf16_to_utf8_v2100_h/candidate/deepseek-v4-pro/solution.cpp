#include "interface.h"
#include "simdutf.h"
#include <memory>

namespace {
uint64_t checksum_bytes(const char* data, size_t len) {
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
  const char16_t* in_ptr = input.data();

  size_t out_len = simdutf::utf8_length_from_utf16le(in_ptr, in_len);
  std::unique_ptr<char[]> buffer(new char[out_len]);

  size_t actual = simdutf::convert_valid_utf16le_to_utf8(in_ptr, in_len, buffer.get());
  return checksum_bytes(buffer.get(), actual);
}