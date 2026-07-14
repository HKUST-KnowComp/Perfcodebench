#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());

  uint64_t hash = 1469598103934665603ULL;

  size_t i = 0;

  // Fast path for long ASCII runs: UTF-8 output is identical to input bytes.
  while (i < n) {
    size_t ascii_start = i;
    while (i < n && src[i] <= 0x7F) {
      ++i;
    }
    for (size_t j = ascii_start; j < i; ++j) {
      hash = fnv1a_update(hash, src[j]);
    }
    if (i == n) break;

    // Process a run containing non-ASCII bytes directly into the checksum,
    // avoiding materializing the UTF-8 output buffer.
    size_t nonascii_start = i;
    while (i < n && src[i] > 0x7F) {
      ++i;
    }
    for (size_t j = nonascii_start; j < i; ++j) {
      const unsigned char ch = src[j];
      hash = fnv1a_update(hash, static_cast<unsigned char>(0xC0U | (ch >> 6)));
      hash = fnv1a_update(hash, static_cast<unsigned char>(0x80U | (ch & 0x3FU)));
    }
  }

  return hash;
}
