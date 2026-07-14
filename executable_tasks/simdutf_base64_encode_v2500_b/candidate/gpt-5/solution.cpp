#include "interface.h"

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t len = input.size();

  uint64_t hash = FNV_OFFSET;

  const char* T = kTable;

  size_t full_blocks = len / 3;
  size_t i = 0;

  // Process full 3-byte blocks
  for (; i < full_blocks; ++i) {
    uint32_t b0 = p[0];
    uint32_t b1 = p[1];
    uint32_t b2 = p[2];
    p += 3;

    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    unsigned char c0 = static_cast<unsigned char>(T[(triple >> 18) & 0x3FU]);
    unsigned char c1 = static_cast<unsigned char>(T[(triple >> 12) & 0x3FU]);
    unsigned char c2 = static_cast<unsigned char>(T[(triple >> 6) & 0x3FU]);
    unsigned char c3 = static_cast<unsigned char>(T[triple & 0x3FU]);

    hash ^= c0; hash *= FNV_PRIME;
    hash ^= c1; hash *= FNV_PRIME;
    hash ^= c2; hash *= FNV_PRIME;
    hash ^= c3; hash *= FNV_PRIME;
  }

  // Handle remaining 1 or 2 bytes
  size_t rem = len - full_blocks * 3;
  if (rem) {
    uint32_t b0 = p[0];
    uint32_t b1 = (rem == 2) ? p[1] : 0U;
    uint32_t triple = (b0 << 16) | (b1 << 8);

    unsigned char c0 = static_cast<unsigned char>(T[(triple >> 18) & 0x3FU]);
    unsigned char c1 = static_cast<unsigned char>(T[(triple >> 12) & 0x3FU]);
    unsigned char c2 = (rem == 2) ? static_cast<unsigned char>(T[(triple >> 6) & 0x3FU])
                                  : static_cast<unsigned char>('=');
    unsigned char c3 = static_cast<unsigned char>('=');

    hash ^= c0; hash *= FNV_PRIME;
    hash ^= c1; hash *= FNV_PRIME;
    hash ^= c2; hash *= FNV_PRIME;
    hash ^= c3; hash *= FNV_PRIME;
  }

  return hash;
}
