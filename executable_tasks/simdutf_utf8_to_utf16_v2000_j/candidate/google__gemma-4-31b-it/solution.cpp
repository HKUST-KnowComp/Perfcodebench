#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

uint64_t compute_checksum(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // Pre-calculate the required size for the UTF-16 output buffer
  // simdutf::convert_utf8_to_utf16 requires a destination buffer of sufficient size.
  // The maximum possible size for UTF-16 is input.size() * 2 (for 1-byte UTF-8 chars),
  // but more accurately, it's input.size() if all are 1-byte, or less if multi-byte.
  // A safe upper bound is input.size() * 2, but we can use simdutf's size estimation if available
  // or just allocate a buffer that is guaranteed to fit.
  
  // For performance, we use a vector to manage memory, but we reserve based on a reasonable heuristic
  // or the worst case. Since we need the exact checksum, correctness is priority.
  std::vector<char16_t> out;
  out.resize(input.size()); // Heuristic: most UTF-8 strings don't expand beyond their byte length in UTF-16

  const char8_t* src = reinterpret_cast<const char8_t*>(input.data());
  size_t src_len = input.size();
  char16_t* dst = out.data();
  size_t dst_len = out.size();

  // Use simdutf for high-performance transcoding
  // convert_utf8_to_utf16 returns the number of UTF-16 code units written
  auto result = simdutf::convert_utf8_to_utf16(src, src_len, dst, dst_len);

  if (result.error == simdutf::utf8_to_utf16_result::error_insufficient_destination_buffer) {
    // Handle buffer overflow by resizing and retrying
    // The required size is provided by the result
    out.resize(result.destination_length);
    result = simdutf::convert_utf8_to_utf16(src, src_len, out.data(), out.size());
  }

  return compute_checksum(out.data(), result.destination_length);
}