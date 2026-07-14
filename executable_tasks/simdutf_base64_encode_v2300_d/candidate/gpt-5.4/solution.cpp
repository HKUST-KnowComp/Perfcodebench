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

  const size_t full = n / 3;
  size_t i = 0;

  for (size_t block = 0; block < full; ++block, i += 3) {
    const uint32_t triple = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8) | uint32_t(src[i + 2]);

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]);
    hash ^= c0;
    hash *= kPrime;

    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]);
    hash ^= c1;
    hash *= kPrime;

    const unsigned char c2 = static_cast<unsigned char>(kTable[(triple >> 6) & 0x3F]);
    hash ^= c2;
    hash *= kPrime;

    const unsigned char c3 = static_cast<unsigned char>(kTable[triple & 0x3F]);
    hash ^= c3;
    hash *= kPrime;
  }

  const size_t rem = n - i;
  if (rem == 1) {
    const uint32_t triple = uint32_t(src[i]) << 16;

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]);
    hash ^= c0;
    hash *= kPrime;

    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]);
    hash ^= c1;
    hash *= kPrime;

    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
  } else if (rem == 2) {
    const uint32_t triple = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8);

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]);
    hash ^= c0;
    hash *= kPrime;

    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]);
    hash ^= c1;
    hash *= kPrime;

    const unsigned char c2 = static_cast<unsigned char>(kTable[(triple >> 6) & 0x3F]);
    hash ^= c2;
    hash *= kPrime;

    hash ^= static_cast<unsigned char>('=');
    hash *= kPrime;
  }

  return hash;
}
