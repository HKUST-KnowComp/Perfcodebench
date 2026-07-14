#include "interface.h"

#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void fnv1a_update(uint64_t &h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= FNV_PRIME;
}

inline void process_triple_and_update(const unsigned char* p, uint64_t &h) {
  const uint32_t b0 = p[0];
  const uint32_t b1 = p[1];
  const uint32_t b2 = p[2];
  const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
  const char c0 = kTable[(triple >> 18) & 0x3FU];
  const char c1 = kTable[(triple >> 12) & 0x3FU];
  const char c2 = kTable[(triple >> 6) & 0x3FU];
  const char c3 = kTable[triple & 0x3FU];
  fnv1a_update(h, static_cast<unsigned char>(c0));
  fnv1a_update(h, static_cast<unsigned char>(c1));
  fnv1a_update(h, static_cast<unsigned char>(c2));
  fnv1a_update(h, static_cast<unsigned char>(c3));
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t n = input.size();
  uint64_t h = FNV_OFFSET;

  const size_t full_groups = n / 3; // number of 3-byte groups
  size_t i = 0;

  // Process in blocks of 4 groups (12 bytes -> 16 output chars)
  const size_t blocks4 = full_groups / 4;
  for (size_t b = 0; b < blocks4; ++b) {
    process_triple_and_update(p + 0, h);
    process_triple_and_update(p + 3, h);
    process_triple_and_update(p + 6, h);
    process_triple_and_update(p + 9, h);
    p += 12;
    i += 4;
  }

  // Process remaining full 3-byte groups
  for (; i < full_groups; ++i) {
    process_triple_and_update(p, h);
    p += 3;
  }

  // Handle remainder (1 or 2 bytes) with padding
  const size_t rem = n % 3;
  if (rem == 1) {
    const uint32_t b0 = p[0];
    const uint32_t triple = (b0 << 16);
    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = '=';
    const char c3 = '=';
    fnv1a_update(h, static_cast<unsigned char>(c0));
    fnv1a_update(h, static_cast<unsigned char>(c1));
    fnv1a_update(h, static_cast<unsigned char>(c2));
    fnv1a_update(h, static_cast<unsigned char>(c3));
  } else if (rem == 2) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t triple = (b0 << 16) | (b1 << 8);
    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = kTable[(triple >> 6) & 0x3FU];
    const char c3 = '=';
    fnv1a_update(h, static_cast<unsigned char>(c0));
    fnv1a_update(h, static_cast<unsigned char>(c1));
    fnv1a_update(h, static_cast<unsigned char>(c2));
    fnv1a_update(h, static_cast<unsigned char>(c3));
  }

  return h;
}
