#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

namespace {

// FNV-1a hash over a byte buffer
uint64_t checksum_bytes(const char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  // Compute exact base64 output length (including padding)
  size_t output_length = ((input.size() + 2) / 3) * 4;
  // Allocate buffer for encoded result
  std::string encoded;
  encoded.resize(output_length);
  // Use simdutf's base64 encoder
  size_t actual_length = simdutf::base64::encode(input.data(), input.size(), &encoded[0]);
  // Compute and return FNV-1a checksum over the encoded bytes
  return checksum_bytes(encoded.data(), actual_length);
}
