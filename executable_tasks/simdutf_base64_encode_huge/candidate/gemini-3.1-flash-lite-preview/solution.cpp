#include "interface.h"
#include <cstdint>
#include <string>
#include <cstring>

namespace {
constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t update_hash(uint64_t hash, uint8_t ch) {
  hash ^= static_cast<uint64_t>(ch);
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = input.size();
  const uint8_t* data = reinterpret_cast<const uint8_t*>(input.data());
  size_t i = 0;

  for (; i + 2 < n; i += 3) {
    uint32_t b0 = data[i];
    uint32_t b1 = data[i + 1];
    uint32_t b2 = data[i + 2];
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    hash = update_hash(hash, kTable[(triple >> 18) & 0x3F]);
    hash = update_hash(hash, kTable[(triple >> 12) & 0x3F]);
    hash = update_hash(hash, kTable[(triple >> 6) & 0x3F]);
    hash = update_hash(hash, kTable[triple & 0x3F]);
  }

  if (i < n) {
    uint32_t b0 = data[i];
    uint32_t b1 = (i + 1 < n) ? data[i + 1] : 0;
    uint32_t b2 = 0;
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    hash = update_hash(hash, kTable[(triple >> 18) & 0x3F]);
    hash = update_hash(hash, kTable[(triple >> 12) & 0x3F]);
    hash = update_hash(hash, (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=');
    hash = update_hash(hash, '=');
  }

  return hash;
}