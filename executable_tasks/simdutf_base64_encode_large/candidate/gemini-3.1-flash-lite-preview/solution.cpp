#include "interface.h"
#include <string>
#include <vector>

namespace {
constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t fnv1a_step(uint64_t hash, char ch) {
  hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ch));
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = input.size();
  const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());

  size_t i = 0;
  for (; i + 2 < n; i += 3) {
    uint32_t triple = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
    hash = fnv1a_step(hash, kTable[(triple >> 18) & 0x3F]);
    hash = fnv1a_step(hash, kTable[(triple >> 12) & 0x3F]);
    hash = fnv1a_step(hash, kTable[(triple >> 6) & 0x3F]);
    hash = fnv1a_step(hash, kTable[triple & 0x3F]);
  }

  if (i < n) {
    uint32_t b0 = data[i];
    uint32_t b1 = (i + 1 < n) ? data[i + 1] : 0;
    uint32_t triple = (b0 << 16) | (b1 << 8);
    
    hash = fnv1a_step(hash, kTable[(triple >> 18) & 0x3F]);
    hash = fnv1a_step(hash, kTable[(triple >> 12) & 0x3F]);
    hash = fnv1a_step(hash, (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=');
    hash = fnv1a_step(hash, '=');
  }

  return hash;
}