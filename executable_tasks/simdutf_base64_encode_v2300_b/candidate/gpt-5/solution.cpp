#include "interface.h"

#include <cstddef>
#include <cstdint>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

constexpr char kTable[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline void fnv_update(uint64_t &hash, uint8_t byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= FNV_PRIME;
}
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(input.data());
  const std::size_t n = input.size();
  uint64_t hash = FNV_OFFSET;

  const std::size_t full = (n / 3) * 3;
  std::size_t i = 0;

  // Process full 3-byte blocks
  for (; i < full; i += 3) {
    const uint32_t triple = (uint32_t(p[i]) << 16) | (uint32_t(p[i + 1]) << 8) | uint32_t(p[i + 2]);

    const char c0 = kTable[(triple >> 18) & 0x3F];
    const char c1 = kTable[(triple >> 12) & 0x3F];
    const char c2 = kTable[(triple >> 6) & 0x3F];
    const char c3 = kTable[triple & 0x3F];

    fnv_update(hash, static_cast<uint8_t>(c0));
    fnv_update(hash, static_cast<uint8_t>(c1));
    fnv_update(hash, static_cast<uint8_t>(c2));
    fnv_update(hash, static_cast<uint8_t>(c3));
  }

  // Handle remaining 1 or 2 bytes with padding
  const std::size_t rem = n - full;
  if (rem == 1) {
    const uint32_t triple = (uint32_t(p[i]) << 16);
    const char c0 = kTable[(triple >> 18) & 0x3F];
    const char c1 = kTable[(triple >> 12) & 0x3F];

    fnv_update(hash, static_cast<uint8_t>(c0));
    fnv_update(hash, static_cast<uint8_t>(c1));
    fnv_update(hash, static_cast<uint8_t>('='));
    fnv_update(hash, static_cast<uint8_t>('='));
  } else if (rem == 2) {
    const uint32_t triple = (uint32_t(p[i]) << 16) | (uint32_t(p[i + 1]) << 8);
    const char c0 = kTable[(triple >> 18) & 0x3F];
    const char c1 = kTable[(triple >> 12) & 0x3F];
    const char c2 = kTable[(triple >> 6) & 0x3F];

    fnv_update(hash, static_cast<uint8_t>(c0));
    fnv_update(hash, static_cast<uint8_t>(c1));
    fnv_update(hash, static_cast<uint8_t>(c2));
    fnv_update(hash, static_cast<uint8_t>('='));
  }

  return hash;
}
