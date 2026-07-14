#include "interface.h"
#include "simdutf.h"
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  // Allocate sufficient space for the decoded binary data.
  std::size_t max_out = simdutf::base64_maximum_decode_length(input);
  std::string binary(max_out, '\0');
  // Decode the base64 input.
  auto result = simdutf::base64_decode(input.data(), input.size(), binary.data());
  // Assume success; the input is guaranteed to be valid base64 with optional padding.
  // Compute and return the checksum over exactly the decoded bytes.
  return checksum_bytes(binary.data(), result.count);
}