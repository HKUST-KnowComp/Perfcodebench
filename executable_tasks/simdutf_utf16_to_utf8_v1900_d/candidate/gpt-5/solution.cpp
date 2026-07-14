#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  // FNV-1a 64-bit parameters
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  const size_t in_len = input.size();
  if (in_len == 0) {
    return fnv_offset_basis;
  }

  // Compute exact UTF-8 length and allocate once
  const size_t utf8_len = simdutf::utf8_length_from_utf16(input.data(), in_len);

  // If there is no output, return the hash of empty sequence
  if (utf8_len == 0) {
    return fnv_offset_basis;
  }

  std::string out;
  out.resize(utf8_len);

  // Perform the transcoding
  size_t written = simdutf::utf16_to_utf8(input.data(), in_len, out.data());
  // For valid input, written should equal utf8_len, but be resilient just in case
  if (written != utf8_len) {
    out.resize(written);
  }

  // Compute FNV-1a over the exact UTF-8 byte sequence
  uint64_t hash = fnv_offset_basis;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(out.data());
  const size_t sz = out.size();
  for (size_t i = 0; i < sz; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= fnv_prime;
  }

  return hash;
}
