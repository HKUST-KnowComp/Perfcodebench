#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

namespace {

inline uint64_t checksum_utf16_ptr(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* end = data + len;
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // The number of UTF-16 code units is at most the number of UTF-8 bytes.
  // For ASCII, it's 1:1. For multi-byte, it's usually fewer units, 
  // except for 4-byte UTF-8 which becomes 2 UTF-16 units (still fewer).
  // We use a stack buffer for small inputs or a unique_ptr for larger ones to avoid vector overhead.
  size_t max_utf16_len = input.size();
  
  // Use a fixed-size buffer on stack for common small strings to avoid heap allocation
  char16_t stack_buf[1024];
  char16_t* heap_buf = nullptr;
  char16_t* target_buf = stack_buf;

  if (max_utf16_len > 1024) {
    heap_buf = new char16_t[max_utf16_len];
    target_buf = heap_buf;
  }

  // Use simdutf for high-performance SIMD transcoding
  size_t actual_utf16_len = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), target_buf);

  uint64_t result = checksum_utf16_ptr(target_buf, actual_utf16_len);

  if (heap_buf) {
    delete[] heap_buf;
  }

  return result;
}