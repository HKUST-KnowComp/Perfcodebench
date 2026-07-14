#include "interface.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= 1099511628211ULL;
  return hash;
}

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());

  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;

  // Process 12 input bytes -> 16 base64 bytes per iteration.
  for (; i + 12 <= n; i += 12) {
    uint32_t v0 = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8) | uint32_t(src[i + 2]);
    uint32_t v1 = (uint32_t(src[i + 3]) << 16) | (uint32_t(src[i + 4]) << 8) | uint32_t(src[i + 5]);
    uint32_t v2 = (uint32_t(src[i + 6]) << 16) | (uint32_t(src[i + 7]) << 8) | uint32_t(src[i + 8]);
    uint32_t v3 = (uint32_t(src[i + 9]) << 16) | (uint32_t(src[i + 10]) << 8) | uint32_t(src[i + 11]);

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v0 >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v0 >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v0 >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[v0 & 0x3F]));

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v1 >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v1 >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v1 >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[v1 & 0x3F]));

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v2 >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v2 >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v2 >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[v2 & 0x3F]));

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v3 >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v3 >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v3 >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[v3 & 0x3F]));
  }

  // Process remaining full 3-byte groups.
  for (; i + 3 <= n; i += 3) {
    uint32_t v = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8) | uint32_t(src[i + 2]);
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[v & 0x3F]));
  }

  // Tail with padding.
  const size_t rem = n - i;
  if (rem == 1) {
    uint32_t v = uint32_t(src[i]) << 16;
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    uint32_t v = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8);
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(v >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
