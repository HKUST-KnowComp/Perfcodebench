#include "interface.h"

#include <string>
#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline uint64_t fnv1a_update(uint64_t hash, unsigned char ch) {
  hash ^= static_cast<uint64_t>(ch);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());

  uint64_t hash = 1469598103934665603ULL;

  size_t i = 0;
  const size_t full = n / 3;

  for (size_t block = 0; block < full; ++block, i += 3) {
    const uint32_t triple = (static_cast<uint32_t>(src[i]) << 16) |
                            (static_cast<uint32_t>(src[i + 1]) << 8) |
                            static_cast<uint32_t>(src[i + 2]);

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[triple & 0x3F]));
  }

  const size_t rem = n - i;
  if (rem == 1) {
    const uint32_t triple = static_cast<uint32_t>(src[i]) << 16;
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const uint32_t triple = (static_cast<uint32_t>(src[i]) << 16) |
                            (static_cast<uint32_t>(src[i + 1]) << 8);
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 18) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 12) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(triple >> 6) & 0x3F]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
