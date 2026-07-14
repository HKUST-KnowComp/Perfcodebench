#include "interface.h"

namespace {
constexpr unsigned char kTable[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

inline void fnv1a_update(uint64_t &hash, unsigned char ch) {
  hash ^= static_cast<uint64_t>(ch);
  hash *= kFNVPrime;
}
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  auto remaining = input.size();
  uint64_t hash = kFNVOffset;

  while (remaining >= 3) {
    uint32_t b0 = p[0];
    uint32_t b1 = p[1];
    uint32_t b2 = p[2];
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    fnv1a_update(hash, kTable[(triple >> 18) & 0x3F]);
    fnv1a_update(hash, kTable[(triple >> 12) & 0x3F]);
    fnv1a_update(hash, kTable[(triple >> 6) & 0x3F]);
    fnv1a_update(hash, kTable[triple & 0x3F]);

    p += 3;
    remaining -= 3;
  }

  if (remaining) {
    uint32_t b0 = p[0];
    uint32_t b1 = (remaining >= 2) ? p[1] : 0U;
    uint32_t triple = (b0 << 16) | (b1 << 8);

    fnv1a_update(hash, kTable[(triple >> 18) & 0x3F]);
    fnv1a_update(hash, kTable[(triple >> 12) & 0x3F]);

    if (remaining == 2) {
      fnv1a_update(hash, kTable[(triple >> 6) & 0x3F]);
      fnv1a_update(hash, static_cast<unsigned char>('='));
    } else { // remaining == 1
      fnv1a_update(hash, static_cast<unsigned char>('='));
      fnv1a_update(hash, static_cast<unsigned char>('='));
    }
  }

  return hash;
}
