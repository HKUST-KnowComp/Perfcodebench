#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>
#include <cstddef>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  // Calculate required output size for standard base64
  std::size_t encoded_len = simdutf::base64_length_from_binary(input.size());
  
  // Allocate output buffer
  std::string out;
  out.resize(encoded_len);
  
  // Encode using simdutf (standard base64 with padding)
  std::size_t actual_len = simdutf::binary_to_base64(
      input.data(),
      input.size(),
      &out[0]
  );
  
  // Compute checksum over the encoded bytes
  return checksum_bytes(out.data(), actual_len);
}