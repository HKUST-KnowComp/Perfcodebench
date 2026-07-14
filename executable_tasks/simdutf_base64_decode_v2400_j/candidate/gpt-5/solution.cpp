#include "interface.h"

#include <string>
#include <cstdint>

namespace {

// Build a decoding table once. Values:
//  0..63: valid base64 values
//    -2 : '=' padding
//    -1 : invalid character
static const int8_t* get_decode_table() {
  static int8_t table[256];
  static bool inited = false;
  if (!inited) {
    for (int i = 0; i < 256; ++i) table[i] = -1;
    for (int i = 'A'; i <= 'Z'; ++i) table[i] = static_cast<int8_t>(i - 'A');
    for (int i = 'a'; i <= 'z'; ++i) table[i] = static_cast<int8_t>(i - 'a' + 26);
    for (int i = '0'; i <= '9'; ++i) table[i] = static_cast<int8_t>(i - '0' + 52);
    table[static_cast<unsigned char>('+')] = 62;
    table[static_cast<unsigned char>('/')] = 63;
    table[static_cast<unsigned char>('=')] = -2;
    inited = true;
  }
  return table;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const int8_t* table = get_decode_table();

  // FNV-1a 64-bit
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = fnv_offset_basis;

  const size_t n = input.size();
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t groups = n / 4; // assume well-formed input; ignore any trailing <4 chars

  for (size_t g = 0; g < groups; ++g) {
    const unsigned char b0 = p[0];
    const unsigned char b1 = p[1];
    const unsigned char b2 = p[2];
    const unsigned char b3 = p[3];

    const int c0 = table[b0];
    const int c1 = table[b1];
    const int c2 = table[b2];
    const int c3 = table[b3];

    // Build 24-bit triple. Mirror baseline behavior for negative values.
    const uint32_t triple = (static_cast<uint32_t>(c0) << 18) |
                            (static_cast<uint32_t>(c1) << 12) |
                            (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
                            static_cast<uint32_t>((c3 < 0) ? 0 : c3);

    // First byte always emitted
    unsigned char out0 = static_cast<unsigned char>((triple >> 16) & 0xFFu);
    hash ^= static_cast<uint64_t>(out0);
    hash *= fnv_prime;

    if (c2 != -2) {
      unsigned char out1 = static_cast<unsigned char>((triple >> 8) & 0xFFu);
      hash ^= static_cast<uint64_t>(out1);
      hash *= fnv_prime;
    }
    if (c3 != -2) {
      unsigned char out2 = static_cast<unsigned char>(triple & 0xFFu);
      hash ^= static_cast<uint64_t>(out2);
      hash *= fnv_prime;
    }

    p += 4;
  }

  return hash;
}
