#include "interface.h"
#include "simdutf.h"

#include <cstddef>
#include <string>

static inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = length;
  size_t i = 0;
  // Unrolled loop for better throughput
  for (; i + 4 <= n; i += 4) {
    hash ^= static_cast<uint64_t>(data[i + 0]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= 1099511628211ULL;
  }
  for (; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t transcode_checksum(const std::string& input) {
  const char* in_ptr = input.data();
  const size_t in_len = input.size();

  // Determine exact UTF-16 code unit count and allocate once
  const size_t out_len = simdutf::utf16_length_from_utf8(in_ptr, in_len);

  if (out_len == 0) {
    // FNV-1 offset basis for empty sequence
    return 1469598103934665603ULL;
  }

  // Allocate output buffer and transcode
  std::u16string out;
  out.resize(out_len);
  const size_t produced = simdutf::convert_utf8_to_utf16(in_ptr, in_len, out.data());

  // Compute checksum over the exact produced UTF-16 code unit sequence
  return checksum_utf16(out.data(), produced);
}
