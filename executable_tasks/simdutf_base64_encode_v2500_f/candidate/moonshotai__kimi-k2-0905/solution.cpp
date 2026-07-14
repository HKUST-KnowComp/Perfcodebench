#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
  // simdutf gives exact required size including padding
  size_t out_len = simdutf::base64_length_from_binary(input.size());
  std::string out(out_len, '\0');

  // Fast SIMD encode
  size_t written = simdutf::binary_to_base64(input.data(), input.size(), out.data());
  out.resize(written); // in case simdutf returns a shorter length (shouldn’t)

  // Compute checksum while string is hot
  return checksum_bytes(out.data(), out.size());
}