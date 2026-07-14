#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {
inline uint64_t fnv1a_utf16(const char16_t* data, size_t length) {
  const uint64_t offset_basis = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint64_t hash = offset_basis;
  const char16_t* end = data + length;
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= prime;
  }
  return hash;
}
}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL; // FNV-1a offset basis
  }

  const size_t out_capacity = input.size(); // worst-case UTF-16 units
  char16_t* out = new char16_t[out_capacity];

  const size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out);
  const uint64_t result = fnv1a_utf16(out, written);

  delete[] out;
  return result;
}
