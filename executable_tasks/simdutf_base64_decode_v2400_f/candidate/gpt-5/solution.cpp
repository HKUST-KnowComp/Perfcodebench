#include "interface.h"

#include <cstdint>
#include <string>

namespace {

// Build a lookup table once for fast base64 character decoding.
static inline const int8_t* base64_lut() {
  static int8_t table[256];
  static bool initialized = []() {
    for (int i = 0; i < 256; ++i) table[i] = -1;
    for (int c = 'A'; c <= 'Z'; ++c) table[c] = static_cast<int8_t>(c - 'A');
    for (int c = 'a'; c <= 'z'; ++c) table[c] = static_cast<int8_t>(c - 'a' + 26);
    for (int c = '0'; c <= '9'; ++c) table[c] = static_cast<int8_t>(c - '0' + 52);
    table[static_cast<unsigned char>('+')] = 62;
    table[static_cast<unsigned char>('/')] = 63;
    table[static_cast<unsigned char>('=')] = -2; // padding
    return true;
  }();
  (void)initialized;
  return table;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const int8_t* lut = base64_lut();

  // FNV-1a 64-bit parameters
  uint64_t hash = 1469598103934665603ULL; // offset basis
  const uint64_t prime = 1099511628211ULL;

  const unsigned char* s = reinterpret_cast<const unsigned char*>(input.data());
  const size_t n = input.size();

  // Process input in full 4-char blocks; valid Base64 input should be a multiple of 4.
  // We guard anyway to avoid OOB if malformed input is provided.
  size_t i = 0;
  const size_t end = n & ~static_cast<size_t>(3); // largest multiple of 4 <= n
  for (; i < end; i += 4) {
    const int v0 = lut[s[i + 0]];
    const int v1 = lut[s[i + 1]];
    const int v2 = lut[s[i + 2]];
    const int v3 = lut[s[i + 3]];

    // Assemble 24-bit group; for negative values, treat as 0 when shifting (matches baseline behavior).
    const uint32_t triple =
        (static_cast<uint32_t>(v0) << 18) |
        (static_cast<uint32_t>(v1) << 12) |
        (static_cast<uint32_t>((v2 < 0) ? 0 : v2) << 6) |
        static_cast<uint32_t>((v3 < 0) ? 0 : v3);

    // First byte is always produced
    uint8_t b0 = static_cast<uint8_t>((triple >> 16) & 0xFFu);
    hash ^= static_cast<uint64_t>(b0);
    hash *= prime;

    // Second byte unless padding '=' at position 2
    if (v2 != -2) {
      uint8_t b1 = static_cast<uint8_t>((triple >> 8) & 0xFFu);
      hash ^= static_cast<uint64_t>(b1);
      hash *= prime;

      // Third byte unless padding '=' at position 3
      if (v3 != -2) {
        uint8_t b2 = static_cast<uint8_t>(triple & 0xFFu);
        hash ^= static_cast<uint64_t>(b2);
        hash *= prime;
      }
    }
  }

  return hash;
}
