#include "interface.h"

#include <string>
#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  // FNV-1a 64-bit initial offset basis
  uint64_t hash = 1469598103934665603ULL;

  const unsigned char* s = reinterpret_cast<const unsigned char*>(input.data());
  const size_t n = input.size();
  const size_t rem = n % 3;
  const size_t n3 = n - rem;  // largest multiple of 3

  // Process full 3-byte blocks
  const unsigned char* p = s;
  const unsigned char* const main_end = s + n3;
  while (p < main_end) {
    const uint32_t triple = (static_cast<uint32_t>(p[0]) << 16) |
                            (static_cast<uint32_t>(p[1]) << 8)  |
                            (static_cast<uint32_t>(p[2]));

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3FU]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3FU]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[(triple >> 6)  & 0x3FU]);
    const unsigned char c3 = static_cast<unsigned char>(kTable[ triple        & 0x3FU]);

    hash ^= c0; hash *= 1099511628211ULL;
    hash ^= c1; hash *= 1099511628211ULL;
    hash ^= c2; hash *= 1099511628211ULL;
    hash ^= c3; hash *= 1099511628211ULL;

    p += 3;
  }

  // Handle tail with standard '=' padding
  if (rem == 1) {
    const uint32_t b0 = static_cast<uint32_t>(s[n3]);
    const uint32_t triple = (b0 << 16);

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3FU]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3FU]);
    const unsigned char c2 = static_cast<unsigned char>('=');
    const unsigned char c3 = static_cast<unsigned char>('=');

    hash ^= c0; hash *= 1099511628211ULL;
    hash ^= c1; hash *= 1099511628211ULL;
    hash ^= c2; hash *= 1099511628211ULL;
    hash ^= c3; hash *= 1099511628211ULL;
  } else if (rem == 2) {
    const uint32_t b0 = static_cast<uint32_t>(s[n3]);
    const uint32_t b1 = static_cast<uint32_t>(s[n3 + 1]);
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3FU]);
    const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3FU]);
    const unsigned char c2 = static_cast<unsigned char>(kTable[(triple >> 6)  & 0x3FU]);
    const unsigned char c3 = static_cast<unsigned char>('=');

    hash ^= c0; hash *= 1099511628211ULL;
    hash ^= c1; hash *= 1099511628211ULL;
    hash ^= c2; hash *= 1099511628211ULL;
    hash ^= c3; hash *= 1099511628211ULL;
  }

  return hash;
}
