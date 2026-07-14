#include "interface.h"

#include <cstddef>

namespace {
constexpr char kTable[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;
}

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  size_t len = input.size();

  uint64_t hash = kFNVOffset;

  // Number of full 3-byte blocks and remainder
  size_t nfull = len / 3;
  size_t rem = len - nfull * 3;

  // Process full blocks, unrolled by 4
  size_t blocks4 = nfull / 4;
  size_t tail_full = nfull % 4;

  while (blocks4--) {
    // First triple
    {
      unsigned b0 = p[0];
      unsigned b1 = p[1];
      unsigned b2 = p[2];
      char c0 = kTable[b0 >> 2];
      char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
      char c2 = kTable[((b1 & 0xF) << 2) | (b2 >> 6)];
      char c3 = kTable[b2 & 0x3F];
      hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
      p += 3;
    }
    // Second triple
    {
      unsigned b0 = p[0];
      unsigned b1 = p[1];
      unsigned b2 = p[2];
      char c0 = kTable[b0 >> 2];
      char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
      char c2 = kTable[((b1 & 0xF) << 2) | (b2 >> 6)];
      char c3 = kTable[b2 & 0x3F];
      hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
      p += 3;
    }
    // Third triple
    {
      unsigned b0 = p[0];
      unsigned b1 = p[1];
      unsigned b2 = p[2];
      char c0 = kTable[b0 >> 2];
      char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
      char c2 = kTable[((b1 & 0xF) << 2) | (b2 >> 6)];
      char c3 = kTable[b2 & 0x3F];
      hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
      p += 3;
    }
    // Fourth triple
    {
      unsigned b0 = p[0];
      unsigned b1 = p[1];
      unsigned b2 = p[2];
      char c0 = kTable[b0 >> 2];
      char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
      char c2 = kTable[((b1 & 0xF) << 2) | (b2 >> 6)];
      char c3 = kTable[b2 & 0x3F];
      hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
      hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
      p += 3;
    }
  }

  // Process remaining full triples
  for (size_t i = 0; i < tail_full; ++i) {
    unsigned b0 = p[0];
    unsigned b1 = p[1];
    unsigned b2 = p[2];
    char c0 = kTable[b0 >> 2];
    char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
    char c2 = kTable[((b1 & 0xF) << 2) | (b2 >> 6)];
    char c3 = kTable[b2 & 0x3F];
    hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
    p += 3;
  }

  // Handle remainder
  if (rem == 1) {
    unsigned b0 = p[0];
    char c0 = kTable[b0 >> 2];
    char c1 = kTable[(b0 & 0x3) << 4];
    char c2 = '=';
    char c3 = '=';
    hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
  } else if (rem == 2) {
    unsigned b0 = p[0];
    unsigned b1 = p[1];
    char c0 = kTable[b0 >> 2];
    char c1 = kTable[((b0 & 0x3) << 4) | (b1 >> 4)];
    char c2 = kTable[(b1 & 0xF) << 2];
    char c3 = '=';
    hash ^= static_cast<unsigned char>(c0); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c1); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c2); hash *= kFNVPrime;
    hash ^= static_cast<unsigned char>(c3); hash *= kFNVPrime;
  }

  return hash;
}
