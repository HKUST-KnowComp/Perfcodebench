#include "interface.h"

#include <string>
#include <cstdint>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline const signed char* base64_table() {
  static signed char table[256];
  static bool inited = false;
  if (!inited) {
    for (int i = 0; i < 256; ++i) table[i] = static_cast<signed char>(-1);
    for (int c = 'A'; c <= 'Z'; ++c) table[c] = static_cast<signed char>(c - 'A');
    for (int c = 'a'; c <= 'z'; ++c) table[c] = static_cast<signed char>(c - 'a' + 26);
    for (int c = '0'; c <= '9'; ++c) table[c] = static_cast<signed char>(c - '0' + 52);
    table[static_cast<unsigned char>('+')] = 62;
    table[static_cast<unsigned char>('/')] = 63;
    table[static_cast<unsigned char>('=')] = static_cast<signed char>(-2);
    inited = true;
  }
  return table;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const signed char* lut = base64_table();

  uint64_t hash = FNV_OFFSET_BASIS;

  // Process 4-byte blocks
  // Assumes well-formed base64 input (length multiple of 4)
  for (size_t i = 0; i < n; i += 4) {
    const int c0 = static_cast<int>(lut[p[i + 0]]);
    const int c1 = static_cast<int>(lut[p[i + 1]]);
    const int c2 = static_cast<int>(lut[p[i + 2]]);
    const int c3 = static_cast<int>(lut[p[i + 3]]);

    const uint32_t triple =
        (static_cast<uint32_t>(c0) << 18) |
        (static_cast<uint32_t>(c1) << 12) |
        (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
        static_cast<uint32_t>((c3 < 0) ? 0 : c3);

    // Byte 0
    unsigned char b0 = static_cast<unsigned char>((triple >> 16) & 0xFFu);
    hash ^= static_cast<uint64_t>(b0);
    hash *= FNV_PRIME;

    // Byte 1 (if not padding '=' at position 2)
    if (c2 != -2) {
      unsigned char b1 = static_cast<unsigned char>((triple >> 8) & 0xFFu);
      hash ^= static_cast<uint64_t>(b1);
      hash *= FNV_PRIME;

      // Byte 2 (if not padding '=' at position 3)
      if (c3 != -2) {
        unsigned char b2 = static_cast<unsigned char>(triple & 0xFFu);
        hash ^= static_cast<uint64_t>(b2);
        hash *= FNV_PRIME;
      }
    }
  }

  return hash;
}
