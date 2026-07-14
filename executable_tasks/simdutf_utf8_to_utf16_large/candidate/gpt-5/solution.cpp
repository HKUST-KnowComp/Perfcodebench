#include "interface.h"
#include "simdutf.h"

#include <cstddef>
#include <memory>

namespace {
inline uint64_t fnv1a_checksum_u16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a 64-bit prime
  size_t i = 0;
  // Unroll by 4 for better throughput
  for (; i + 4 <= length; i += 4) {
    hash ^= static_cast<uint64_t>(data[i + 0]); hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 1]); hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 2]); hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 3]); hash *= prime;
  }
  for (; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}
} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t len = input.size();
  if (len == 0) {
    return 1469598103934665603ULL; // FNV-1a offset basis for empty input
  }

  // Upper bound: number of UTF-16 code units is <= number of UTF-8 bytes
  std::unique_ptr<char16_t[]> buffer(new char16_t[len]);

  // Convert valid UTF-8 to UTF-16LE using simdutf (fast path, no validation)
  const size_t written = simdutf::convert_valid_utf8_to_utf16le(
      input.data(), len, buffer.get());

  // Compute FNV-1a checksum over the UTF-16 code unit sequence
  return fnv1a_checksum_u16(buffer.get(), written);
}
