#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  const char* end = data + len;
  
  // Process 8 bytes at a time
  while (data + 8 <= end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[0]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[1]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[2]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[3]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[4]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[5]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[6]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[7]));
    hash *= prime;
    data += 8;
  }
  
  // Handle remaining bytes
  while (data < end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
    hash *= prime;
  }
  
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  
  // Calculate required UTF-8 buffer size
  size_t utf8_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), input.size());
  
  // Allocate buffer for UTF-8 output
  std::string out;
  out.resize(utf8_len);
  
  // Perform SIMD-accelerated transcoding
  size_t written = simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(),
      out.data());
  
  return checksum_bytes(out.data(), written);
}