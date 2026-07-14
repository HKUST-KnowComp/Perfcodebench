#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>
#include <cstddef>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char* end = data + len;
  while (data < end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;  // FNV offset basis for empty input
  }
  
  // Calculate required output size for base64 encoding
  size_t output_len = simdutf::base64_length_from_binary(input.size());
  
  // Allocate output buffer
  std::string out;
  out.resize(output_len);
  
  // Encode using simdutf (standard base64 with padding)
  size_t written = simdutf::binary_to_base64(
      input.data(),
      input.size(),
      out.data()
  );
  
  return checksum_bytes(out.data(), written);
}