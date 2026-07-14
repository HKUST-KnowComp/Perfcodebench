#include "interface.h"

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

#include <simdutf.h>

namespace {

// FNV-1a 64-bit hash over a byte range
uint64_t fnv1a_64(const char* data, size_t len) noexcept {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint8_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // anonymous namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    // FNV-1a of empty string is the initial offset basis
    return 1469598103934665603ULL;
  }

  // Determine exact UTF-8 output size
  const size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());

  // Allocate output buffer (std::vector for contiguous memory)
  std::vector<char> output(utf8_len);

  // Transcode UTF-16LE to UTF-8
  const size_t written = simdutf::utf16le_to_utf8(input.data(), input.size(), output.data());
  (void)written; // written == utf8_len on success

  // Compute checksum over the UTF-8 bytes
  return fnv1a_64(output.data(), utf8_len);
}