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

uint64_t encode_checksum(const std::string& input) {
  size_t output_size = ((input.size() + 2) / 3) * 4;
  std::vector<char> output(output_size);
  size_t encoded_size = simdutf::base64_encode(input.data(), input.size(), output.data());
  return checksum_bytes(output.data(), output_size);
}