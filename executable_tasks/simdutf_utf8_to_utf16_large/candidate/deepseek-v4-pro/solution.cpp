#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  // Allocate space for worst-case output size (input.size() UTF-16 code units).
  std::vector<char16_t> out(input.size());
  // Perform SIMD-accelerated transcoding from UTF-8 to UTF-16LE.
  size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  // Compute checksum over the output sequence.
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < out_len; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}