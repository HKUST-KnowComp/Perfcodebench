#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {

// Initialize base64 decode table once, mapping bytes to values in [0..63],
// with -2 for '=' padding and -1 for invalid bytes.
inline const int8_t* base64_decode_table() {
  static int8_t table[256];
  static bool inited = false;
  if (!inited) {
    // default to invalid
    for (int i = 0; i < 256; ++i) table[i] = -1;
    // 'A'-'Z' -> 0..25
    for (int c = 'A'; c <= 'Z'; ++c) table[c] = static_cast<int8_t>(c - 'A');
    // 'a'-'z' -> 26..51
    for (int c = 'a'; c <= 'z'; ++c) table[c] = static_cast<int8_t>(c - 'a' + 26);
    // '0'-'9' -> 52..61
    for (int c = '0'; c <= '9'; ++c) table[c] = static_cast<int8_t>(c - '0' + 52);
    table[static_cast<unsigned char>('+')] = 62;
    table[static_cast<unsigned char>('/')] = 63;
    table[static_cast<unsigned char>('=')] = -2; // padding
    inited = true;
  }
  return table;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const int8_t* const table = base64_decode_table();
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const std::size_t n = input.size();

  // FNV-1a 64-bit
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  // Process in groups of 4 input chars -> up to 3 output bytes
  // Assumes input length is a multiple of 4, same as baseline.
  for (std::size_t i = 0; i < n; i += 4) {
    const int c0 = static_cast<int>(table[p[i]]);
    const int c1 = static_cast<int>(table[p[i + 1]]);
    const int c2 = static_cast<int>(table[p[i + 2]]);
    const int c3 = static_cast<int>(table[p[i + 3]]);

    const uint32_t triple = (static_cast<uint32_t>(c0) << 18) |
                            (static_cast<uint32_t>(c1) << 12) |
                            (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
                            static_cast<uint32_t>((c3 < 0) ? 0 : c3);

    // First byte always emitted
    const unsigned char b0 = static_cast<unsigned char>((triple >> 16) & 0xFFu);
    hash ^= static_cast<uint64_t>(b0);
    hash *= fnv_prime;

    // Second byte emitted unless padding '=' at position 2
    if (c2 != -2) {
      const unsigned char b1 = static_cast<unsigned char>((triple >> 8) & 0xFFu);
      hash ^= static_cast<uint64_t>(b1);
      hash *= fnv_prime;
    }

    // Third byte emitted unless padding '=' at position 3
    if (c3 != -2) {
      const unsigned char b2 = static_cast<unsigned char>(triple & 0xFFu);
      hash ^= static_cast<uint64_t>(b2);
      hash *= fnv_prime;
    }
  }

  return hash;
}
