#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <cstddef>

namespace {
inline uint64_t fnv1a64(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const uint8_t* end = data + len;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}
} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t len = input.size();
  if (len == 0) {
    return 1469598103934665603ULL; // checksum of empty output
  }

  // Safe upper bound for UTF-8 bytes from UTF-16 input: 3 bytes per 16-bit unit
  const size_t needed_capacity = len * 3;

  static thread_local char* buffer = nullptr;
  static thread_local size_t buffer_capacity = 0;

  if (needed_capacity > buffer_capacity) {
    delete[] buffer;
    buffer = new char[needed_capacity];
    buffer_capacity = needed_capacity;
  }

  // Convert using simdutf (input is guaranteed valid)
  const size_t written = simdutf::convert_valid_utf16_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), len, buffer);

  return fnv1a64(reinterpret_cast<const uint8_t*>(buffer), written);
}
