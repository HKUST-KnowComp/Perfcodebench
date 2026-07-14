#include "interface.h"

#include <cstddef>
#include <cstdint>
#include "simdutf.h"

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();
  if (len == 0) {
    // Checksum over empty UTF-16 sequence
    return 1469598103934665603ULL;
  }

  // Reusable thread-local buffer to avoid repeated allocations/initializations
  thread_local char16_t* buffer = nullptr;
  thread_local size_t capacity = 0;

  const size_t max_units = len; // UTF-16 units <= number of UTF-8 bytes
  if (capacity < max_units) {
    delete[] buffer;
    buffer = new char16_t[max_units]; // default-initialized (no zeroing for fundamental types)
    capacity = max_units;
  }

  // Transcode using simdutf to native-endian UTF-16 code units (char16_t)
  size_t out_len = simdutf::utf8_to_utf16(src, len, buffer);

  return checksum_utf16(buffer, out_len);
}
