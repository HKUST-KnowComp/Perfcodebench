#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  const size_t input_len = input.size();
  if (input_len == 0) return 1469598103934665603ULL;

  // simdutf::utf8_to_utf32_length provides an upper bound
  size_t max_out = simdutf::utf8_to_utf32_length(input.data(), input_len);
  std::vector<char32_t> out(max_out);

  // Transcode using simdutf
  size_t actual_len = simdutf::convert_utf8_to_utf32(input.data(), input_len, out.data());

  // Compute checksum in-place
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < actual_len; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}