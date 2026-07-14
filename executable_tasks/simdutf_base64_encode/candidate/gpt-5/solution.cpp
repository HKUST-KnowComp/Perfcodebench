#include "interface.h"

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

inline void fnv1a_update(uint64_t &hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= kFNVPrime;
}

inline void encode_triple_and_update(const unsigned char b0,
                                     const unsigned char b1,
                                     const unsigned char b2,
                                     uint64_t &hash) {
  const unsigned i0 = b0 >> 2;
  const unsigned i1 = ((b0 & 0x03u) << 4) | (b1 >> 4);
  const unsigned i2 = ((b1 & 0x0Fu) << 2) | (b2 >> 6);
  const unsigned i3 = b2 & 0x3Fu;

  fnv1a_update(hash, static_cast<unsigned char>(kTable[i0]));
  fnv1a_update(hash, static_cast<unsigned char>(kTable[i1]));
  fnv1a_update(hash, static_cast<unsigned char>(kTable[i2]));
  fnv1a_update(hash, static_cast<unsigned char>(kTable[i3]));
}

} // namespace

uint64_t encode_checksum(const std::string &input) {
  const unsigned char *src = reinterpret_cast<const unsigned char *>(input.data());
  const size_t len = input.size();

  uint64_t hash = kFNVOffset;

  size_t i = 0;
  // Unroll by 4 triples (12 bytes -> 16 output chars)
  if (len >= 12) {
    const size_t unroll_end = len / 12 * 12; // largest multiple of 12
    for (; i < unroll_end; i += 12) {
      // Process 4 triples
      encode_triple_and_update(src[i + 0], src[i + 1], src[i + 2], hash);
      encode_triple_and_update(src[i + 3], src[i + 4], src[i + 5], hash);
      encode_triple_and_update(src[i + 6], src[i + 7], src[i + 8], hash);
      encode_triple_and_update(src[i + 9], src[i + 10], src[i + 11], hash);
    }
  }

  // Process remaining full triples
  for (; i + 2 < len; i += 3) {
    encode_triple_and_update(src[i], src[i + 1], src[i + 2], hash);
  }

  // Handle padding for remaining 1 or 2 bytes
  const size_t rem = len - i;
  if (rem == 1) {
    const unsigned char b0 = src[i];
    const unsigned i0 = b0 >> 2;
    const unsigned i1 = (b0 & 0x03u) << 4; // b1 == 0

    fnv1a_update(hash, static_cast<unsigned char>(kTable[i0]));
    fnv1a_update(hash, static_cast<unsigned char>(kTable[i1]));
    fnv1a_update(hash, static_cast<unsigned char>('='));
    fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const unsigned char b0 = src[i];
    const unsigned char b1 = src[i + 1];
    const unsigned i0 = b0 >> 2;
    const unsigned i1 = ((b0 & 0x03u) << 4) | (b1 >> 4);
    const unsigned i2 = (b1 & 0x0Fu) << 2; // b2 == 0

    fnv1a_update(hash, static_cast<unsigned char>(kTable[i0]));
    fnv1a_update(hash, static_cast<unsigned char>(kTable[i1]));
    fnv1a_update(hash, static_cast<unsigned char>(kTable[i2]));
    fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
