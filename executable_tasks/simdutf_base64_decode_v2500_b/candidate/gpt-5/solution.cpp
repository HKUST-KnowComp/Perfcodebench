#include "interface.h"

#include <cstdint>
#include <string>

namespace {

// Build a decoding table once: maps byte to [0..63], -2 for '=', -1 for invalid
inline const int8_t* base64_dec_table() {
  static int8_t table[256];
  static bool inited = false;
  if (!inited) {
    for (int i = 0; i < 256; ++i) table[i] = -1;
    for (int c = 'A'; c <= 'Z'; ++c) table[c] = static_cast<int8_t>(c - 'A');
    for (int c = 'a'; c <= 'z'; ++c) table[c] = static_cast<int8_t>(c - 'a' + 26);
    for (int c = '0'; c <= '9'; ++c) table[c] = static_cast<int8_t>(c - '0' + 52);
    table[static_cast<unsigned char>('+')] = 62;
    table[static_cast<unsigned char>('/')] = 63;
    table[static_cast<unsigned char>('=')] = -2;
    inited = true;
  }
  return table;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const int8_t* dt = base64_dec_table();

  const char* p = input.data();
  const size_t n = input.size();

  constexpr uint64_t fnv_offset = 1469598103934665603ULL;
  constexpr uint64_t fnv_prime  = 1099511628211ULL;
  uint64_t hash = fnv_offset;

  // Process in 4-char blocks
  for (size_t i = 0; i + 4 <= n; i += 4) {
    const int c0 = dt[static_cast<unsigned char>(p[i])];
    const int c1 = dt[static_cast<unsigned char>(p[i + 1])];
    const int c2 = dt[static_cast<unsigned char>(p[i + 2])];
    const int c3 = dt[static_cast<unsigned char>(p[i + 3])];

    const uint32_t triple = (static_cast<uint32_t>(c0) << 18)
                          | (static_cast<uint32_t>(c1) << 12)
                          | (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6)
                          |  static_cast<uint32_t>((c3 < 0) ? 0 : c3);

    // First byte always emitted
    uint8_t b0 = static_cast<uint8_t>((triple >> 16) & 0xFFu);
    hash ^= static_cast<uint64_t>(b0);
    hash *= fnv_prime;

    if (c2 != -2) {
      uint8_t b1 = static_cast<uint8_t>((triple >> 8) & 0xFFu);
      hash ^= static_cast<uint64_t>(b1);
      hash *= fnv_prime;
    }
    if (c3 != -2) {
      uint8_t b2 = static_cast<uint8_t>(triple & 0xFFu);
      hash ^= static_cast<uint64_t>(b2);
      hash *= fnv_prime;
    }
  }

  return hash;
}
