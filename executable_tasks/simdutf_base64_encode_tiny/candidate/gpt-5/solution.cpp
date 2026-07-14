#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const std::size_t n = input.size();
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  const char* table = kTable;

  const std::size_t full = n / 3;
  const std::size_t rem = n % 3;

  for (std::size_t i = 0; i < full; ++i) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t b2 = p[2];
    p += 3;

    const unsigned char o0 = static_cast<unsigned char>(table[(b0 >> 2) & 0x3FU]);
    const unsigned char o1 = static_cast<unsigned char>(table[((b0 & 0x3U) << 4) | (b1 >> 4)]);
    const unsigned char o2 = static_cast<unsigned char>(table[((b1 & 0xFU) << 2) | (b2 >> 6)]);
    const unsigned char o3 = static_cast<unsigned char>(table[b2 & 0x3FU]);

    hash ^= o0; hash *= 1099511628211ULL;
    hash ^= o1; hash *= 1099511628211ULL;
    hash ^= o2; hash *= 1099511628211ULL;
    hash ^= o3; hash *= 1099511628211ULL;
  }

  if (rem == 1) {
    const uint32_t b0 = p[0];
    const unsigned char o0 = static_cast<unsigned char>(table[b0 >> 2]);
    const unsigned char o1 = static_cast<unsigned char>(table[(b0 & 0x3U) << 4]);
    const unsigned char eq = static_cast<unsigned char>('=');

    hash ^= o0; hash *= 1099511628211ULL;
    hash ^= o1; hash *= 1099511628211ULL;
    hash ^= eq; hash *= 1099511628211ULL;
    hash ^= eq; hash *= 1099511628211ULL;
  } else if (rem == 2) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const unsigned char o0 = static_cast<unsigned char>(table[b0 >> 2]);
    const unsigned char o1 = static_cast<unsigned char>(table[((b0 & 0x3U) << 4) | (b1 >> 4)]);
    const unsigned char o2 = static_cast<unsigned char>(table[(b1 & 0xFU) << 2]);
    const unsigned char eq = static_cast<unsigned char>('=');

    hash ^= o0; hash *= 1099511628211ULL;
    hash ^= o1; hash *= 1099511628211ULL;
    hash ^= o2; hash *= 1099511628211ULL;
    hash ^= eq; hash *= 1099511628211ULL;
  }

  return hash;
}
