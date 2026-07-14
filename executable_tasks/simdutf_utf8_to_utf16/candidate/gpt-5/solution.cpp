#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <vector>

namespace {
inline uint64_t checksum_utf16_units(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a prime
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}
}

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t src_len = input.size();

  // Precompute exact UTF-16 output length to allocate once.
  const size_t out_len = simdutf::utf16_length_from_utf8(src, src_len);
  std::vector<char16_t> out(out_len);

  // Convert using the fast path for valid UTF-8 input.
  const size_t written = simdutf::convert_valid_utf8_to_utf16le(src, src_len, out.data());

  // Compute checksum over the produced UTF-16 code units.
  return checksum_utf16_units(out.data(), written);
}
