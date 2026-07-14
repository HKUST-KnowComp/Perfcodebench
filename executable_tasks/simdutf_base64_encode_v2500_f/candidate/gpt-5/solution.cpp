#include "interface.h"

#include <cstdint>
#include <string>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Standard Base64 table
constexpr unsigned char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline void hash_update(uint64_t &h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= FNV_PRIME;
}

inline void encode3_hash(const unsigned char *p, uint64_t &h) {
  const uint32_t triple = (uint32_t(p[0]) << 16) | (uint32_t(p[1]) << 8) | uint32_t(p[2]);
  const unsigned char c0 = kTable[(triple >> 18) & 0x3F];
  const unsigned char c1 = kTable[(triple >> 12) & 0x3F];
  const unsigned char c2 = kTable[(triple >> 6) & 0x3F];
  const unsigned char c3 = kTable[triple & 0x3F];
  hash_update(h, c0);
  hash_update(h, c1);
  hash_update(h, c2);
  hash_update(h, c3);
}
}  // namespace

uint64_t encode_checksum(const std::string &input) {
  const unsigned char *p = reinterpret_cast<const unsigned char *>(input.data());
  size_t n = input.size();
  uint64_t h = FNV_OFFSET;

  size_t full = n / 3;
  size_t rem = n - full * 3;

  // Unroll by 4 groups of 3 bytes (12 bytes -> 16 base64 chars)
  while (full >= 4) {
    encode3_hash(p, h);
    encode3_hash(p + 3, h);
    encode3_hash(p + 6, h);
    encode3_hash(p + 9, h);
    p += 12;
    full -= 4;
  }
  while (full > 0) {
    encode3_hash(p, h);
    p += 3;
    --full;
  }

  if (rem) {
    const unsigned char b0 = p[0];
    if (rem == 1) {
      const unsigned char c0 = kTable[(b0 >> 2) & 0x3F];
      const unsigned char c1 = kTable[(b0 & 0x3) << 4];
      hash_update(h, c0);
      hash_update(h, c1);
      hash_update(h, '=');
      hash_update(h, '=');
    } else {  // rem == 2
      const unsigned char b1 = p[1];
      const unsigned char c0 = kTable[(b0 >> 2) & 0x3F];
      const unsigned char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
      const unsigned char c2 = kTable[(b1 & 0x0F) << 2];
      hash_update(h, c0);
      hash_update(h, c1);
      hash_update(h, c2);
      hash_update(h, '=');
    }
  }

  return h;
}
