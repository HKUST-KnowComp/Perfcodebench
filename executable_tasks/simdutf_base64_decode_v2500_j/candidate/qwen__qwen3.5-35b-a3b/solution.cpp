#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "simdutf.h"

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

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes(nullptr, 0);
  }

  size_t max_output_size = (input.size() / 4) * 3;
  std::vector<char> output(max_output_size);

  size_t decoded_size = simdutf::base64_decode(
      input.data(), input.size(), output.data(), output.size());

  return checksum_bytes(output.data(), decoded_size);
}