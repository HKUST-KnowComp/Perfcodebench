#include "interface.h"

#include <string>
#include <cstddef>
#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// FNV-1a 64-bit constants
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline void fnv1a_update(uint64_t& hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= kFNVPrime;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t n = input.size();
  const char* table = kTable;

  uint64_t hash = kFNVOffset;

  // Process full 3-byte blocks
  const size_t n_full = (n / 3) * 3;
  const unsigned char* const end = p + n_full;

  while (p != end) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t b2 = p[2];

    const unsigned o0 = (b0 >> 2);
    const unsigned o1 = ((b0 & 0x3u) << 4) | (b1 >> 4);
    const unsigned o2 = ((b1 & 0xFu) << 2) | (b2 >> 6);
    const unsigned o3 = (b2 & 0x3Fu);

    const unsigned char c0 = static_cast<unsigned char>(table[o0]);
    const unsigned char c1 = static_cast<unsigned char>(table[o1]);
    const unsigned char c2 = static_cast<unsigned char>(table[o2]);
    const unsigned char c3 = static_cast<unsigned char>(table[o3]);

    fnv1a_update(hash, c0);
    fnv1a_update(hash, c1);
    fnv1a_update(hash, c2);
    fnv1a_update(hash, c3);

    p += 3;
  }

  // Handle remaining 1 or 2 bytes
  const size_t rem = n - n_full;
  if (rem == 1) {
    const uint32_t b0 = p[0];
    const unsigned o0 = (b0 >> 2);
    const unsigned o1 = ((b0 & 0x3u) << 4); // b1==0

    const unsigned char c0 = static_cast<unsigned char>(table[o0]);
    const unsigned char c1 = static_cast<unsigned char>(table[o1]);

    fnv1a_update(hash, c0);
    fnv1a_update(hash, c1);
    fnv1a_update(hash, static_cast<unsigned char>('='));
    fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const unsigned o0 = (b0 >> 2);
    const unsigned o1 = ((b0 & 0x3u) << 4) | (b1 >> 4);
    const unsigned o2 = ((b1 & 0xFu) << 2); // b2==0

    const unsigned char c0 = static_cast<unsigned char>(table[o0]);
    const unsigned char c1 = static_cast<unsigned char>(table[o1]);
    const unsigned char c2 = static_cast<unsigned char>(table[o2]);

    fnv1a_update(hash, c0);
    fnv1a_update(hash, c1);
    fnv1a_update(hash, c2);
    fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
