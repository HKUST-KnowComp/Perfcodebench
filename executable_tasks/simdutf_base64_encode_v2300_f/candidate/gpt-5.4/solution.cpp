#include "interface.h"

#include <cstdint>
#include <string>

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
  const size_t full_bytes = full * 3;

  for (; i < full_bytes; i += 3) {
    const uint32_t b0 = src[i];
    const uint32_t b1 = src[i + 1];
    const uint32_t b2 = src[i + 2];

    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[b0 >> 2]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[((b0 & 0x03u) << 4) | (b1 >> 4)]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[((b1 & 0x0Fu) << 2) | (b2 >> 6)]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[b2 & 0x3Fu]));
  }

  const size_t rem = n - full_bytes;
  if (rem == 1) {
    const uint32_t b0 = src[i];
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[b0 >> 2]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(b0 & 0x03u) << 4]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const uint32_t b0 = src[i];
    const uint32_t b1 = src[i + 1];
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[b0 >> 2]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[((b0 & 0x03u) << 4) | (b1 >> 4)]));
    hash = fnv1a_update(hash, static_cast<unsigned char>(kTable[(b1 & 0x0Fu) << 2]));
    hash = fnv1a_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
