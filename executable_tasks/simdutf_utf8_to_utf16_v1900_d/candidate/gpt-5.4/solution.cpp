#include "interface.h"

#include "simdutf.h"

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::string& input) {
  const size_t n = input.size();
  const char* src = input.data();

  // Fast path: ASCII-only input. UTF-16 code units equal byte values.
  bool ascii = true;
  size_t i = 0;
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  for (; i + sizeof(uint64_t) <= n; i += sizeof(uint64_t)) {
    uint64_t v;
    __builtin_memcpy(&v, src + i, sizeof(uint64_t));
    if (v & 0x8080808080808080ULL) {
      ascii = false;
      break;
    }
  }
  if (ascii) {
    for (; i < n; ++i) {
      if (static_cast<unsigned char>(src[i]) & 0x80U) {
        ascii = false;
        break;
      }
    }
  }

  if (ascii) {
    uint64_t hash = kOffset;
    for (size_t j = 0; j < n; ++j) {
      hash ^= static_cast<unsigned char>(src[j]);
      hash *= kPrime;
    }
    return hash;
  }

  const size_t utf16_len = simdutf::utf16_length_from_utf8(src, n);
  char16_t* buffer = new char16_t[utf16_len];
  simdutf::convert_valid_utf8_to_utf16le(src, n, buffer);

  uint64_t hash = kOffset;
  for (size_t j = 0; j < utf16_len; ++j) {
    hash ^= static_cast<uint64_t>(buffer[j]);
    hash *= kPrime;
  }

  delete[] buffer;
  return hash;
}
