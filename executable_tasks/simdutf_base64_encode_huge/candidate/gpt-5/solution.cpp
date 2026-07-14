#include "interface.h"

#include <cstdint>
#include <string>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;
}

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t len = input.size();
  uint64_t hash = kFNVOffset;

  const size_t full = len / 3;
  const size_t rem = len - full * 3;

  const char* table = kTable;

  const unsigned char* end_full = p + full * 3;
  while (p != end_full) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t b2 = static_cast<uint32_t>(p[2]);
    p += 3;

    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c0);
    hash *= kFNVPrime;

    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c1);
    hash *= kFNVPrime;

    unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c2);
    hash *= kFNVPrime;

    unsigned char c3 = static_cast<unsigned char>(table[triple & 0x3FU]);
    hash ^= static_cast<uint64_t>(c3);
    hash *= kFNVPrime;
  }

  if (rem == 1) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t triple = (b0 << 16);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c0);
    hash *= kFNVPrime;

    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c1);
    hash *= kFNVPrime;

    // '=' padding
    unsigned char eq = static_cast<unsigned char>('=');
    hash ^= static_cast<uint64_t>(eq);
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(eq);
    hash *= kFNVPrime;
  } else if (rem == 2) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c0);
    hash *= kFNVPrime;

    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c1);
    hash *= kFNVPrime;

    unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3FU]);
    hash ^= static_cast<uint64_t>(c2);
    hash *= kFNVPrime;

    // '=' padding
    unsigned char eq = static_cast<unsigned char>('=');
    hash ^= static_cast<uint64_t>(eq);
    hash *= kFNVPrime;
  }

  return hash;
}
