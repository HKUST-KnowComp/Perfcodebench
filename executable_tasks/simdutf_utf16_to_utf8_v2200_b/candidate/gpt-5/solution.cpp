#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstddef>
#include <cstdint>

namespace {

inline uint64_t fnv1a64(const char* data, size_t len) {
  constexpr uint64_t offset_basis = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  uint64_t hash = offset_basis;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= prime;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* in = input.data();
  const size_t in_len = input.size();
  // Compute the exact number of UTF-8 bytes required.
  const size_t out_len = simdutf::utf8_length_from_utf16(in, in_len);

  if (out_len == 0) {
    // Return FNV-1a offset basis for empty output.
    return 1469598103934665603ULL;
  }

  std::string out;
  out.resize(out_len);
  // Convert assuming valid UTF-16 input for maximum speed.
  size_t written = simdutf::convert_valid_utf16_to_utf8(in, in_len, out.data());
  // Compute checksum over the exact bytes produced.
  // In valid cases, written should equal out_len; we hash the produced bytes.
  return fnv1a64(out.data(), written);
}
