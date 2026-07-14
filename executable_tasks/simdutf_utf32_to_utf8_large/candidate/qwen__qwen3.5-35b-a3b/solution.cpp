#include "interface.h"

#include <vector>
#include <cstdint>
#include "simdutf.h"

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  std::vector<char> output_buffer(input.size() * 4);
  size_t bytes_written = simdutf::utf32_to_utf8(input.data(), input.size(), output_buffer.data());

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < bytes_written; ++i) {
    unsigned char ch = static_cast<unsigned char>(output_buffer[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}