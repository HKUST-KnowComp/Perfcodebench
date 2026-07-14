#include "interface.h"

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const auto* data = reinterpret_cast<const unsigned char*>(input.data());
  const std::size_t n = input.size();

  uint64_t hash = 1469598103934665603ULL;
  std::size_t i = 0;

  // Process full 3-byte groups (no padding)
  while (i + 3 <= n) {
    const uint32_t b0 = data[i];
    const uint32_t b1 = data[i + 1];
    const uint32_t b2 = data[i + 2];
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    unsigned char c = kTable[(triple >> 18) & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;
    c = kTable[(triple >> 12) & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;
    c = kTable[(triple >> 6) & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;
    c = kTable[triple & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;

    i += 3;
  }

  // Handle remaining 1 or 2 bytes (tail with padding)
  if (i < n) {
    const uint32_t b0 = data[i];
    const uint32_t b1 = (i + 1 < n) ? data[i + 1] : 0U;
    const uint32_t b2 = 0U;
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    unsigned char c = kTable[(triple >> 18) & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;
    c = kTable[(triple >> 12) & 0x3FU];
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;

    if (i + 1 < n) {
      c = kTable[(triple >> 6) & 0x3FU];
    } else {
      c = '=';
    }
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;

    c = '=';  // last char is always padding for tail
    hash ^= static_cast<uint64_t>(c); hash *= 1099511628211ULL;
  }

  return hash;
}