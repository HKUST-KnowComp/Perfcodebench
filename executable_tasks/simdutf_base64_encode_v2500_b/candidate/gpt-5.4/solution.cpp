#include "interface.h"

#include <cstdint>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());

  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  size_t i = 0;
  const size_t full = n / 3;

  for (size_t block = 0; block < full; ++block, i += 3) {
    const uint32_t b0 = src[i];
    const uint32_t b1 = src[i + 1];
    const uint32_t b2 = src[i + 2];

    const unsigned char c0 = static_cast<unsigned char>(kTable[b0 >> 2]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[((b0 & 0x03u) << 4) | (b1 >> 4)]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[((b1 & 0x0Fu) << 2) | (b2 >> 6)]);
    const unsigned char c3 = static_cast<unsigned char>(kTable[b2 & 0x3Fu]);

    hash ^= c0;
    hash *= kPrime;
    hash ^= c1;
    hash *= kPrime;
    hash ^= c2;
    hash *= kPrime;
    hash ^= c3;
    hash *= kPrime;
  }

  const size_t rem = n - i;
  if (rem == 1) {
    const uint32_t b0 = src[i];
    const unsigned char c0 = static_cast<unsigned char>(kTable[b0 >> 2]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[(b0 & 0x03u) << 4]);
    hash ^= c0;
    hash *= kPrime;
    hash ^= c1;
    hash *= kPrime;
    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
  } else if (rem == 2) {
    const uint32_t b0 = src[i];
    const uint32_t b1 = src[i + 1];
    const unsigned char c0 = static_cast<unsigned char>(kTable[b0 >> 2]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[((b0 & 0x03u) << 4) | (b1 >> 4)]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[(b1 & 0x0Fu) << 2]);
    hash ^= c0;
    hash *= kPrime;
    hash ^= c1;
    hash *= kPrime;
    hash ^= c2;
    hash *= kPrime;
    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
  }

  return hash;
}
