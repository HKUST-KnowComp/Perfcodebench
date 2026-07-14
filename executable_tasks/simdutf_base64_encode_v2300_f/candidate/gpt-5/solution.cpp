#include "interface.h"

#include <cstdint>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline void fnv1a_byte(uint64_t &h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= kFNVPrime;
}

// Process exactly 3 input bytes into 4 base64 bytes and update FNV-1a hash.
inline void encode3_hash(const unsigned char* p, uint64_t &h) {
  const uint32_t b0 = p[0];
  const uint32_t b1 = p[1];
  const uint32_t b2 = p[2];
  const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
  const char* T = kTable;
  fnv1a_byte(h, static_cast<unsigned char>(T[(triple >> 18) & 0x3FU]));
  fnv1a_byte(h, static_cast<unsigned char>(T[(triple >> 12) & 0x3FU]));
  fnv1a_byte(h, static_cast<unsigned char>(T[(triple >> 6) & 0x3FU]));
  fnv1a_byte(h, static_cast<unsigned char>(T[triple & 0x3FU]));
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  size_t n = input.size();

  uint64_t hash = kFNVOffset;

  // Unrolled main loop: process 12 bytes (4 triples) per iteration when possible.
  size_t n_full = n / 3; // number of full triples
  size_t blocks4 = n_full / 4; // number of 4-triple blocks
  for (size_t i = 0; i < blocks4; ++i) {
    encode3_hash(p + 0, hash);
    encode3_hash(p + 3, hash);
    encode3_hash(p + 6, hash);
    encode3_hash(p + 9, hash);
    p += 12;
  }

  // Remaining full triples (0..3)
  size_t rem_triples = n_full - blocks4 * 4;
  for (size_t i = 0; i < rem_triples; ++i) {
    encode3_hash(p, hash);
    p += 3;
  }

  // Tail: handle 1 or 2 leftover bytes
  size_t rem = n - (n_full * 3);
  if (rem) {
    const char* T = kTable;
    if (rem == 1) {
      const uint32_t b0 = p[0];
      const unsigned char c0 = static_cast<unsigned char>(T[b0 >> 2]);
      const unsigned char c1 = static_cast<unsigned char>(T[(b0 & 0x3U) << 4]);
      fnv1a_byte(hash, c0);
      fnv1a_byte(hash, c1);
      fnv1a_byte(hash, static_cast<unsigned char>('='));
      fnv1a_byte(hash, static_cast<unsigned char>('='));
    } else { // rem == 2
      const uint32_t b0 = p[0];
      const uint32_t b1 = p[1];
      const unsigned char c0 = static_cast<unsigned char>(T[b0 >> 2]);
      const unsigned char c1 = static_cast<unsigned char>(T[((b0 & 0x3U) << 4) | (b1 >> 4)]);
      const unsigned char c2 = static_cast<unsigned char>(T[(b1 & 0xFU) << 2]);
      fnv1a_byte(hash, c0);
      fnv1a_byte(hash, c1);
      fnv1a_byte(hash, c2);
      fnv1a_byte(hash, static_cast<unsigned char>('='));
    }
  }

  return hash;
}
