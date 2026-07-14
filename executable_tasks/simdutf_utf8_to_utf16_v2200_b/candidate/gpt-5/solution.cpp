#include "interface.h"
#include "simdutf.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t count) {
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime  = 1099511628211ULL;
  uint64_t hash = fnv_offset;
  for (size_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= fnv_prime;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* in = input.data();
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL; // FNV-1a offset basis for empty sequence
  }

  // UTF-16 code units produced are <= input bytes count.
  std::unique_ptr<char16_t[]> out(new char16_t[in_len]);
  size_t written = simdutf::convert_utf8_to_utf16le(in, in_len, out.get());
  // Input is guaranteed valid per task; 'written' is the number of UTF-16 code units.
  return checksum_utf16(out.get(), written);
}
