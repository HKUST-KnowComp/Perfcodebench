#include "interface.h"

#include <cstdint>
#include <string>

namespace {

// Returns a pointer to a 256-entry lookup table for Base64 decoding.
// Mapping:
//  - 'A'-'Z' -> 0..25
//  - 'a'-'z' -> 26..51
//  - '0'-'9' -> 52..61
//  - '+' -> 62
//  - '/' -> 63
//  - '=' -> 0xFE (padding)
//  - others -> 0xFF (invalid)
inline const uint8_t* b64_table() {
  static uint8_t tbl[256];
  static bool inited = false;
  if (!inited) {
    for (int i = 0; i < 256; ++i) tbl[i] = 0xFF;
    for (int c = 'A'; c <= 'Z'; ++c) tbl[c] = static_cast<uint8_t>(c - 'A');
    for (int c = 'a'; c <= 'z'; ++c) tbl[c] = static_cast<uint8_t>(c - 'a' + 26);
    for (int c = '0'; c <= '9'; ++c) tbl[c] = static_cast<uint8_t>(c - '0' + 52);
    tbl[static_cast<unsigned char>('+')] = 62;
    tbl[static_cast<unsigned char>('/')] = 63;
    tbl[static_cast<unsigned char>('=')] = 0xFE; // padding
    inited = true;
  }
  return tbl;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t len = input.size();
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(input.data());
  const uint8_t* T = b64_table();

  // FNV-1a 64-bit
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  // Assume valid Base64 input; process in 4-char blocks.
  // If length is not a multiple of 4, safely handle the remaining part by guarding accesses.
  size_t i = 0;
  const size_t full = (len / 4) * 4;
  for (; i < full; i += 4) {
    uint8_t c0 = ptr[i + 0];
    uint8_t c1 = ptr[i + 1];
    uint8_t c2 = ptr[i + 2];
    uint8_t c3 = ptr[i + 3];

    uint8_t v0 = T[c0];
    uint8_t v1 = T[c1];
    uint8_t v2 = T[c2];
    uint8_t v3 = T[c3];

    // Compute bytes: b0 always valid
    uint8_t b0 = static_cast<uint8_t>((v0 << 2) | (v1 >> 4));
    hash ^= static_cast<uint64_t>(b0);
    hash *= FNV_PRIME;

    if (v2 != 0xFE) { // not padding
      uint8_t b1 = static_cast<uint8_t>(((v1 & 0x0F) << 4) | (v2 >> 2));
      hash ^= static_cast<uint64_t>(b1);
      hash *= FNV_PRIME;

      if (v3 != 0xFE) { // not padding
        uint8_t b2 = static_cast<uint8_t>(((v2 & 0x03) << 6) | v3);
        hash ^= static_cast<uint64_t>(b2);
        hash *= FNV_PRIME;
      }
    }
  }

  // Handle any trailing incomplete block conservatively (should not occur for valid base64).
  if (i < len) {
    // Build a safe 4-tuple with padding for missing characters.
    uint8_t c0 = ptr[i + 0];
    uint8_t c1 = (i + 1 < len) ? ptr[i + 1] : static_cast<uint8_t>('=');
    uint8_t c2 = (i + 2 < len) ? ptr[i + 2] : static_cast<uint8_t>('=');
    uint8_t c3 = (i + 3 < len) ? ptr[i + 3] : static_cast<uint8_t>('=');

    const uint8_t* T2 = T;
    uint8_t v0 = T2[c0];
    uint8_t v1 = T2[c1];
    uint8_t v2 = T2[c2];
    uint8_t v3 = T2[c3];

    uint8_t b0 = static_cast<uint8_t>((v0 << 2) | (v1 >> 4));
    hash ^= static_cast<uint64_t>(b0);
    hash *= FNV_PRIME;

    if (v2 != 0xFE) {
      uint8_t b1 = static_cast<uint8_t>(((v1 & 0x0F) << 4) | (v2 >> 2));
      hash ^= static_cast<uint64_t>(b1);
      hash *= FNV_PRIME;
      if (v3 != 0xFE) {
        uint8_t b2 = static_cast<uint8_t>(((v2 & 0x03) << 6) | v3);
        hash ^= static_cast<uint64_t>(b2);
        hash *= FNV_PRIME;
      }
    }
  }

  return hash;
}
