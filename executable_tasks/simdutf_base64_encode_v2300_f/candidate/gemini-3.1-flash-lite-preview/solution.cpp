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
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = input.size();
  const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());

  for (size_t i = 0; i < n; i += 3) {
    uint32_t b0 = data[i];
    uint32_t b1 = (i + 1 < n) ? data[i + 1] : 0;
    uint32_t b2 = (i + 2 < n) ? data[i + 2] : 0;

    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    char c0 = kTable[(triple >> 18) & 0x3F];
    char c1 = kTable[(triple >> 12) & 0x3F];
    char c2 = (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=';
    char c3 = (i + 2 < n) ? kTable[triple & 0x3F] : '=';

    hash = update_hash(hash, c0);
    hash = update_hash(hash, c1);
    hash = update_hash(hash, c2);
    hash = update_hash(hash, c3);
  }

  return hash;
}