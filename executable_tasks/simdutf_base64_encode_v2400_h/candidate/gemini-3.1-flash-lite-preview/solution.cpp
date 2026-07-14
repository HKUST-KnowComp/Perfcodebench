#include "interface.h"
#include <string>
#include <vector>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t update_hash(uint64_t hash, char ch) {
  hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ch));
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  if (n == 0) return 1469598103934665603ULL;

  size_t out_len = ((n + 2) / 3) * 4;
  uint64_t hash = 1469598103934665603ULL;

  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());
  size_t i = 0;
  for (; i + 2 < n; i += 3) {
    uint32_t triple = (src[i] << 16) | (src[i + 1] << 8) | src[i + 2];
    char c0 = kTable[(triple >> 18) & 0x3F];
    char c1 = kTable[(triple >> 12) & 0x3F];
    char c2 = kTable[(triple >> 6) & 0x3F];
    char c3 = kTable[triple & 0x3F];
    hash = update_hash(update_hash(update_hash(update_hash(hash, c0), c1), c2), c3);
  }

  if (i < n) {
    uint32_t b0 = src[i];
    uint32_t b1 = (i + 1 < n) ? src[i + 1] : 0;
    uint32_t triple = (b0 << 16) | (b1 << 8);
    char c0 = kTable[(triple >> 18) & 0x3F];
    char c1 = kTable[(triple >> 12) & 0x3F];
    char c2 = (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=';
    char c3 = '=';
    hash = update_hash(update_hash(update_hash(update_hash(hash, c0), c1), c2), c3);
  }

  return hash;
}