#include "interface.h"

#include <string>
#include <cstdint>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t encode_checksum(const std::string& input) {
  const uint8_t* s = reinterpret_cast<const uint8_t*>(input.data());
  const size_t n = input.size();
  const char* table = kTable;

  uint64_t hash = FNV_OFFSET_BASIS;

  const size_t full = (n / 3) * 3;
  size_t i = 0;
  for (; i < full; i += 3) {
    const uint32_t b0 = s[i];
    const uint32_t b1 = s[i + 1];
    const uint32_t b2 = s[i + 2];
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
    unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3F]);
    unsigned char c3 = static_cast<unsigned char>(table[triple & 0x3F]);

    hash ^= static_cast<uint64_t>(c0); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c1); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c2); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c3); hash *= FNV_PRIME;
  }

  const size_t rem = n - full;
  if (rem == 1) {
    const uint32_t b0 = s[full];
    const uint32_t triple = (b0 << 16);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
    unsigned char eq = static_cast<unsigned char>('=');

    hash ^= static_cast<uint64_t>(c0); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c1); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(eq); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(eq); hash *= FNV_PRIME;
  } else if (rem == 2) {
    const uint32_t b0 = s[full];
    const uint32_t b1 = s[full + 1];
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
    unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3F]);
    unsigned char eq = static_cast<unsigned char>('=');

    hash ^= static_cast<uint64_t>(c0); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c1); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(c2); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(eq); hash *= FNV_PRIME;
  }

  return hash;
}