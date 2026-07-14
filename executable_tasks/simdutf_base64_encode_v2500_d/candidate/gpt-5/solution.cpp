#include "interface.h"

#include <cstdint>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t fnv1a_update(uint64_t hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= 1099511628211ULL; // FNV prime
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const char* table = kTable;

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  // Process full 3-byte blocks
  size_t i = 0;
  const size_t main_bytes = (n / 3) * 3;
  while (i < main_bytes) {
    const uint32_t b0 = p[i];
    const uint32_t b1 = p[i + 1];
    const uint32_t b2 = p[i + 2];
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    char o0 = table[(triple >> 18) & 0x3FU];
    char o1 = table[(triple >> 12) & 0x3FU];
    char o2 = table[(triple >> 6) & 0x3FU];
    char o3 = table[triple & 0x3FU];

    hash = fnv1a_update(hash, static_cast<unsigned char>(o0));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o1));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o2));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o3));

    i += 3;
  }

  // Handle remaining 1 or 2 bytes
  const size_t rem = n - main_bytes;
  if (rem == 1) {
    const uint32_t b0 = p[i];
    const uint32_t triple = (b0 << 16);

    char o0 = table[(triple >> 18) & 0x3FU];
    char o1 = table[(triple >> 12) & 0x3FU];

    hash = fnv1a_update(hash, static_cast<unsigned char>(o0));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o1));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const uint32_t b0 = p[i];
    const uint32_t b1 = p[i + 1];
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    char o0 = table[(triple >> 18) & 0x3FU];
    char o1 = table[(triple >> 12) & 0x3FU];
    char o2 = table[(triple >> 6) & 0x3FU];

    hash = fnv1a_update(hash, static_cast<unsigned char>(o0));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o1));
    hash = fnv1a_update(hash, static_cast<unsigned char>(o2));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
