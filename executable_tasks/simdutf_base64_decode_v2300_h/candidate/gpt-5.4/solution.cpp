#include "interface.h"

#include <cstdint>
#include <string>

namespace {

static constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
static constexpr uint64_t kFnvPrime = 1099511628211ULL;

static constexpr unsigned char kInvalid = 0xFF;
static constexpr unsigned char kPad = 0xFE;

static constexpr unsigned char kDecodeTable[256] = {
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,62,kInvalid,kInvalid,kInvalid,63,
    52,53,54,55,56,57,58,59,
    60,61,kInvalid,kInvalid,kInvalid,kPad,kInvalid,kInvalid,
    kInvalid,0,1,2,3,4,5,6,
    7,8,9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,
    23,24,25,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,26,27,28,29,30,31,32,
    33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,
    49,50,51,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,
    kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid,kInvalid
};

inline void fnv1a_add(uint64_t& hash, uint8_t byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= kFnvPrime;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* s = reinterpret_cast<const unsigned char*>(input.data());

  uint64_t hash = kFnvOffset;

  if (n == 0) return hash;

  size_t i = 0;
  const size_t full_limit = (n >= 4) ? (n - 4) : 0;

  for (; i < full_limit; i += 4) {
    const uint32_t a = kDecodeTable[s[i]];
    const uint32_t b = kDecodeTable[s[i + 1]];
    const uint32_t c = kDecodeTable[s[i + 2]];
    const uint32_t d = kDecodeTable[s[i + 3]];

    const uint32_t triple = (a << 18) | (b << 12) | (c << 6) | d;
    fnv1a_add(hash, static_cast<uint8_t>(triple >> 16));
    fnv1a_add(hash, static_cast<uint8_t>(triple >> 8));
    fnv1a_add(hash, static_cast<uint8_t>(triple));
  }

  if (i + 4 <= n) {
    const uint32_t a = kDecodeTable[s[i]];
    const uint32_t b = kDecodeTable[s[i + 1]];
    const uint32_t c = kDecodeTable[s[i + 2]];
    const uint32_t d = kDecodeTable[s[i + 3]];

    if (c == kPad) {
      const uint32_t triple = (a << 18) | (b << 12);
      fnv1a_add(hash, static_cast<uint8_t>(triple >> 16));
    } else if (d == kPad) {
      const uint32_t triple = (a << 18) | (b << 12) | (c << 6);
      fnv1a_add(hash, static_cast<uint8_t>(triple >> 16));
      fnv1a_add(hash, static_cast<uint8_t>(triple >> 8));
    } else {
      const uint32_t triple = (a << 18) | (b << 12) | (c << 6) | d;
      fnv1a_add(hash, static_cast<uint8_t>(triple >> 16));
      fnv1a_add(hash, static_cast<uint8_t>(triple >> 8));
      fnv1a_add(hash, static_cast<uint8_t>(triple));
    }
  }

  return hash;
}
