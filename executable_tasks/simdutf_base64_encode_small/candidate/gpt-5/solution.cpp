#include "interface.h"

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const std::size_t n = input.size();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  constexpr uint64_t prime = 1099511628211ULL; // FNV-1a 64-bit prime

  const std::size_t full = n / 3;
  for (std::size_t i = 0; i < full; ++i) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t b2 = p[2];
    p += 3;

    const unsigned char c0 = static_cast<unsigned char>(kTable[(b0 >> 2) & 0x3F]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[((b0 & 0x3) << 4) | (b1 >> 4)]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[((b1 & 0xF) << 2) | (b2 >> 6)]);
    const unsigned char c3 = static_cast<unsigned char>(kTable[b2 & 0x3F]);

    hash ^= c0; hash *= prime;
    hash ^= c1; hash *= prime;
    hash ^= c2; hash *= prime;
    hash ^= c3; hash *= prime;
  }

  const std::size_t rem = n - full * 3;
  if (rem == 1) {
    const uint32_t b0 = p[0];
    const unsigned char c0 = static_cast<unsigned char>(kTable[(b0 >> 2) & 0x3F]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[(b0 & 0x3) << 4]);
    const unsigned char eq = '=';

    hash ^= c0; hash *= prime;
    hash ^= c1; hash *= prime;
    hash ^= eq; hash *= prime;
    hash ^= eq; hash *= prime;
  } else if (rem == 2) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const unsigned char c0 = static_cast<unsigned char>(kTable[(b0 >> 2) & 0x3F]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[((b0 & 0x3) << 4) | (b1 >> 4)]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[(b1 & 0xF) << 2]);
    const unsigned char eq = '=';

    hash ^= c0; hash *= prime;
    hash ^= c1; hash *= prime;
    hash ^= c2; hash *= prime;
    hash ^= eq; hash *= prime;
  }

  return hash;
}
