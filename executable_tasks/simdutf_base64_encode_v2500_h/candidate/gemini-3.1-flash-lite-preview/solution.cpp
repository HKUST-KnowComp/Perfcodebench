#include "interface.h"
#include <string>
#include <vector>

namespace {

constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t update_checksum(uint64_t hash, char ch) {
  hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ch));
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  size_t n = input.size();
  uint64_t hash = 1469598103934665603ULL;

  size_t i = 0;
  for (; i + 2 < n; i += 3) {
    uint32_t b0 = static_cast<unsigned char>(input[i]);
    uint32_t b1 = static_cast<unsigned char>(input[i + 1]);
    uint32_t b2 = static_cast<unsigned char>(input[i + 2]);
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    char c0 = kTable[(triple >> 18) & 0x3F];
    char c1 = kTable[(triple >> 12) & 0x3F];
    char c2 = kTable[(triple >> 6) & 0x3F];
    char c3 = kTable[triple & 0x3F];

    hash = update_checksum(update_checksum(update_checksum(update_checksum(hash, c0), c1), c2), c3);
  }

  if (i < n) {
    uint32_t b0 = static_cast<unsigned char>(input[i]);
    uint32_t b1 = (i + 1 < n) ? static_cast<unsigned char>(input[i + 1]) : 0;
    uint32_t b2 = 0;
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    char c0 = kTable[(triple >> 18) & 0x3F];
    char c1 = kTable[(triple >> 12) & 0x3F];
    char c2 = (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=';
    char c3 = '=';

    hash = update_checksum(update_checksum(update_checksum(update_checksum(hash, c0), c1), c2), c3);
  }

  return hash;
}