#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input);

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // Calculate expected output size
  std::size_t out_size = (input.size() / 4) * 3;
  
  // Validate base64 input
  auto result = simdutf::validate_base64(input.data(), input.size());
  if (result.error != simdutf::error_code::SUCCESS) {
    return checksum_bytes("");
  }

  // Allocate output buffer
  std::vector<char> decoded(out_size);
  
  // Decode using simdutf SIMD-accelerated base64 decoder
  size_t bytes_decoded = simdutf::base64_decode(input.data(), input.size(), decoded.data());
  
  // Calculate checksum over decoded bytes
  return checksum_bytes(std::string(decoded.data(), bytes_decoded));
}