#include "interface.h"
#include "simdutf.h"

#include <memory>
#include <cstddef>

namespace {

inline uint64_t fnv1a_checksum(const unsigned char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    // FNV-1a of an empty byte sequence
    return 1469598103934665603ULL;
  }

  // Determine exact number of UTF-8 bytes required
  const size_t out_len = simdutf::utf8_length_from_utf16(input.data(), in_len);

  // Allocate uninitialized buffer to avoid zero-fill overhead
  std::unique_ptr<char[]> out(new char[out_len]);

  // Perform the conversion
  const size_t written = simdutf::convert_utf16_to_utf8(input.data(), in_len, out.get());

  // Compute checksum over the exact UTF-8 bytes
  return fnv1a_checksum(reinterpret_cast<const unsigned char*>(out.get()), written);
}
