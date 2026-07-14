#include "interface.h"

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(input.data());
  const std::size_t n = input.size();

  uint64_t hash = FNV_OFFSET;

  // Process full 3-byte chunks
  std::size_t full = n / 3;
  for (std::size_t i = 0; i < full; ++i) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t b2 = p[2];
    p += 3;

    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = kTable[(triple >> 6)  & 0x3FU];
    const char c3 = kTable[triple & 0x3FU];

    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c0));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c1));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c2));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c3));
    hash *= FNV_PRIME;
  }

  // Handle remainder (1 or 2 bytes)
  const std::size_t rem = n - full * 3;
  if (rem == 1) {
    const uint32_t b0 = p[0];
    const uint32_t triple = (b0 << 16);

    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = '=';
    const char c3 = '=';

    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c0));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c1));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c2));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c3));
    hash *= FNV_PRIME;
  } else if (rem == 2) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = kTable[(triple >> 6) & 0x3FU];
    const char c3 = '=';

    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c0));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c1));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c2));
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c3));
    hash *= FNV_PRIME;
  }

  return hash;
}
