#include "interface.h"

#include <cstdint>
#include <cstddef>

namespace {

// Standard Base64 encoding table
static constexpr char kTable[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// FNV-1a 64-bit hash update per byte
static inline void fnv1a_update(uint64_t &h, unsigned char byte) {
  h ^= static_cast<uint64_t>(byte);
  h *= 1099511628211ULL;
}

// Emit 4 base64 chars from a 24-bit triple and update hash (no padding)
static inline void emit4_no_pad(uint32_t triple, const char *table, uint64_t &h) {
  unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
  unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
  unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3F]);
  unsigned char c3 = static_cast<unsigned char>(table[triple & 0x3F]);
  fnv1a_update(h, c0);
  fnv1a_update(h, c1);
  fnv1a_update(h, c2);
  fnv1a_update(h, c3);
}

}  // namespace

uint64_t encode_checksum(const std::string &input) {
  const unsigned char *p = reinterpret_cast<const unsigned char *>(input.data());
  const size_t n = input.size();
  const char *table = kTable;

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  // Process full 3-byte chunks
  const size_t full_bytes = (n / 3) * 3;

  size_t i = 0;

  // Unroll by 4 triples (12 bytes) for throughput when possible
  for (; i + 12 <= full_bytes; i += 12) {
    uint32_t t0 = (static_cast<uint32_t>(p[i]) << 16) |
                  (static_cast<uint32_t>(p[i + 1]) << 8) |
                  (static_cast<uint32_t>(p[i + 2]));
    emit4_no_pad(t0, table, hash);

    uint32_t t1 = (static_cast<uint32_t>(p[i + 3]) << 16) |
                  (static_cast<uint32_t>(p[i + 4]) << 8) |
                  (static_cast<uint32_t>(p[i + 5]));
    emit4_no_pad(t1, table, hash);

    uint32_t t2 = (static_cast<uint32_t>(p[i + 6]) << 16) |
                  (static_cast<uint32_t>(p[i + 7]) << 8) |
                  (static_cast<uint32_t>(p[i + 8]));
    emit4_no_pad(t2, table, hash);

    uint32_t t3 = (static_cast<uint32_t>(p[i + 9]) << 16) |
                  (static_cast<uint32_t>(p[i + 10]) << 8) |
                  (static_cast<uint32_t>(p[i + 11]));
    emit4_no_pad(t3, table, hash);
  }

  // Process remaining full triples
  for (; i + 3 <= full_bytes; i += 3) {
    uint32_t triple = (static_cast<uint32_t>(p[i]) << 16) |
                      (static_cast<uint32_t>(p[i + 1]) << 8) |
                      (static_cast<uint32_t>(p[i + 2]));
    emit4_no_pad(triple, table, hash);
  }

  // Handle tail (1 or 2 remaining bytes)
  const size_t rem = n - full_bytes;
  if (rem == 1) {
    // b0 present, b1=b2=0
    const uint32_t b0 = p[full_bytes];
    const uint32_t triple = (b0 << 16);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
    // Two padding characters '='
    fnv1a_update(hash, c0);
    fnv1a_update(hash, c1);
    fnv1a_update(hash, static_cast<unsigned char>('='));
    fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    // b0,b1 present, b2=0
    const uint32_t b0 = p[full_bytes];
    const uint32_t b1 = p[full_bytes + 1];
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    unsigned char c0 = static_cast<unsigned char>(table[(triple >> 18) & 0x3F]);
    unsigned char c1 = static_cast<unsigned char>(table[(triple >> 12) & 0x3F]);
    unsigned char c2 = static_cast<unsigned char>(table[(triple >> 6) & 0x3F]);
    fnv1a_update(hash, c0);
    fnv1a_update(hash, c1);
    fnv1a_update(hash, c2);
    fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
