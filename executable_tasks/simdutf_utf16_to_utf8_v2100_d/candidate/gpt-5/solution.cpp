#include "interface.h"

#include <string>
#include <cstdint>
#include "simdutf.h"

namespace {

inline uint64_t fnv1a_checksum(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* in = input.data();
  const size_t in_len = input.size();

  // Compute exact UTF-8 length and allocate once.
  const size_t out_len = simdutf::utf8_length_from_utf16(in, in_len);

  if (out_len == 0) {
    // Checksum of empty output
    return 1469598103934665603ULL; // FNV-1a offset basis
  }

  std::string out;
  out.resize(out_len);

  // Convert using the fast "valid" path.
  size_t written = simdutf::convert_valid_utf16_to_utf8(in, in_len, &out[0]);

  // Compute checksum over the produced bytes.
  return fnv1a_checksum(out.data(), written);
}
