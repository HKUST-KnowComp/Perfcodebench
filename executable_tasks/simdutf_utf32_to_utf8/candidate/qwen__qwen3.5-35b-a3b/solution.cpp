#pragma once

#include <cstdint>
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  size_t max_bytes = input.size() * 4;
  std::vector<char> buffer(max_bytes);
  size_t bytes_written = simdutf::utf32_to_utf8(input.data(), input.size(), buffer.data());

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < bytes_written; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}